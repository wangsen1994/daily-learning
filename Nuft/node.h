//
// Created by 王森 on 2019-06-27.
//

#include <string>
#include <mutex>
#include <vector>
#include <thread>
#include "Base/stiring_utils.h"
#include "Base/lang_extend.h"

#ifndef NUFT_NODE_H
#define NUFT_NODE_H

#endif //NUFT_NODE_H


typedef std::string Nodename;
typedef int64_t IndexID;
typedef uint64_t TermID;

/*const 变量的初始化可以延迟到运行时，而 constexpr 变量必须在编译时进行初始化*/
static constexpr IndexID default_index_cursor = -1;  /* cursor 光标 */
static constexpr TermID default_term_cursor = 0;
static constexpr uint64_t default_timeout_interval_lowerbound = 150+300;
static constexpr uint64_t default_timeout_interval_upperbound = 300+300;
static constexpr uint64_t default_heartbeat_interval = 30;

static constexpr uint64_t default_selection_interval = 550;

#define vote_for_none "";
#define GUARD std::lock_guard<std::mutex>guard((mut));

#if defined(USE_GRPC_ASYNC)
// Use Async gRPC model
typedef RaftMessagesClientAsync RaftMessagesClient;
#else
// Use Sync gRPC model
#if defined(USE_GRPC_STREAM)
typedef RaftMessagesStreamClientSync RaftMessagesClient;
#else
typedef RaftMessagesClientSync RaftMessagesClient;
#endif
#define GRPC_SYNC_CONCUR_LEVEL 8
#endif

#define SEQ_START 1
#define USE_MORE_REMOVE


struct Persister{

    struct RaftNode* node = nullptr;
    void Dump(std::lock_guard<std::mutex>&, bool backup_conf= false); /* 倾倒 */
    void Load(std::lock_guard<std::mutex>&);

};

struct NodePeer{

    Nodename name;
    bool vote_for_me = false;
    IndexID next_index = default_index_cursor+1; // Index of next log to copy
    IndexID match_index = default_index_cursor;  // Index of logs already copied
    std::shared_ptr<RaftMessagesClient> raft_message_client; // send message to perr

    bool receiver_enabled = true;
    bool send_enabled = true;

    bool voting_right = true;

    uint64_t seq = SEQ_START;  // for next rpc call

};

enum Nuft_Result{

    Nuft_OK = 0,
    Nuft_FAIL ,
    Nuft_RETRY ,
    Nuft_NOT_LEADER,
    Nuft_RESULT_SIZE
};

enum Nuft_CB_Type{

    Nuft_CB_ELECTION_START = 0,
    Nuft_CB_ELECTION_END,
    Nuft_CB_STATE_CHANGE,
    Nuft_CB_ON_APPLY,
    Nuft_CB_CONF_START,
    Nuft_CB_CONF_END,
    NUFT_CB_ON_REPLICATE,
    NUFT_CB_ON_NEW_ENTRY,
    NUFT_CB_SIZE
};

enum Nuft_CMD_Type{

    Nuft_CMD_NORMAL = 0,
    Nuft_CMD_TRANS,
    Nuft_CMD_SNAPSHOT,
    Nuft_CMD_TRANS_NEW,
    Nuft_CMD_SIZE
};

struct NuftCallbackArg{

    struct RaftNode* node;
    std::unique_lock<std::mutex>* lck = nullptr;

    int tmp1 = 0;
    int tmp2 = 0;

    void * ptr = nullptr;
};

typedef int NuftResult;
typedef std::function<NuftResult (Nuft_CB_Type,NuftCallbackArg*)>NuftCallbackFunc;

struct ApplyMessage{

    IndexID index = default_index_cursor;
    TermID term = default_term_cursor;
    std::string name;
    bool from_snapshot = false;
    std::string data;
};

struct RaftNode{

    enum NodeState{
        Follower = 0,
        Candidate,
        Leader,
        NotRunning
    };

    enum SelectState{
        S_NONE = 0, // other state
        S_NEW,      // start
        S_AGA,      // re-start selection self
        S_VOTE,     // vote for others
        S_SUC,      // success
        S_T,        // my selection is timeout
        S_FAIL,     // lose selection as a candidate
        S_SUC_B     // observed other's selection success
    };

    static const char* get_state(NodeState st){

        switch (st){

            case NodeState ::Follower: return "Follwer";
            case NodeState ::Candidate: return "Candidate";
            case NodeState ::Leader: return "Leader";
            case NodeState ::NotRunning: return "NotRunning";
            default: return "ERROR";
        }
    }
    struct Configuration{

        static Configuration from_string(const std::string& conf_str){

            std::vector<std::string> v = Base::split(conf_str,"\n");
            std::vector<std::string> app = Base::split(v[0],";");
            std::vector<std::string> rem = Base::split(v[1],";");
            std::vector<std::string> old = Base::split(v[2],";");

            return {app,rem,old};

        }

        static std::string to_string(const Configuration& conf){

            std::string conf_str;

            conf_str += Base::join(conf.app.begin(),conf.app.end(),";") + "\n";
            conf_str += Base::join(conf.rem.begin(),conf.rem.end(),";") + "\n";
            conf_str += Base::join(conf.old.begin(),conf.old.end(),";") + "\n";

            return conf_str;
        }

        void init(){

            oldvote_thres = old.size();
            newvote_thres = oldvote_thres;

            for(auto& it:app){

                if(!Base::contains(old,it)){newvote_thres++;}
            }

            for(auto& it:rem){
                if(Base::contains(old,it)){ newvote_thres--;}
            }

        }

        Configuration(
                const std::vector<std::string>& app_,
                const std::vector<std::string>& rem_,
                const std::vector<Nodename>& old_):app(app_),rem(rem_),old(old_){
            init();
        }

        Configuration(
                const std::vector<std::string>& app_,
                const std::vector<std::string>& rem_,
                const std::map<Nodename,NodePeer*>old_, const Nodename& leader_exclusive)
                :app(app_),rem(rem_){

            for(auto&& it: old_){
                old.push_back(it.first);
            }
            old.push_back(leader_exclusive);
            init();
        }

        std::vector<std::string> app;
        std::vector<std::string> rem;
        std::vector<std::string> old;


        IndexID  index1 = -1;
        IndexID  index2 = -1;
        size_t oldvote_thres = 0;
        size_t newvote_thres = 0;

        enum State{

            BLANK = 0,
            STAGING,
            OLD_JOINT,
            JOINT,
            NEW_JOINT,
            NEW
        };

        State state = STAGING;

        bool is_in_old(const std::string& peer_name) const {
            return Base::contains(old,peer_name);
        }

        bool is_in_new(const std::string& peer_name) const {
            return (is_in_old(peer_name)&&!Base::contains(rem,peer_name))||Base::contains(app,peer_name);
        }

        bool is_in_joint(const std::string& peer_name) const {

            return is_in_old(peer_name)||is_in_new(peer_name);
        }
    };

    TermID current_term = default_index_cursor;
    Nodename vote_for = vote_for_none;
    std::vector<raft_message::LogEntry> logs;
    std::string leader_name;
    struct Configuration* trans_conf = nullptr;
    struct Persister* persister = nullptr;

    // RSM usage
    IndexID  commit_index = default_index_cursor;
    IndexID  last_applied = default_index_cursor;

    // Leader exclusive

    // Node information
    Nodename name;
    NodeState state;
    bool paused = false;
    bool tobe_destructed = false;
    uint64_t last_tick = 0;                 // Reset every `default_heartbeat_interval`
    uint64_t elect_timeout_due = 0;         // At this point the Follower start election
    uint64_t election_fail_timeout_due = 0; // At this point the Candidate's election timeout

    // candidate exclusive 独占
    size_t vote_got = 0;
    size_t new_vote = 0;
    size_t old_vote = 0;

    // Network
    std::map<Nodename,NodePeer*> peers;
    std::vector<NodePeer*> tobe_removed_peers;
#if defined(USE_GRPC_STREAM)
    RaftStreamServerContext* raft_message_server = nullptr;
#else
    RaftServerContext* raft_message_server = nullptr;
#endif
    std::thread timer_thread;
    uint64_t start_timepoint;  // TODO Reject RPC from previous test cases.
    uint64_t last_timepoint;   // TODO Reject RPC from previous meeage from current Leader.
    uint64_t last_seq;

#if defined(USE_GRPC_SYNC)&&!defined(USE_GRPC_STREAM)&&!defined(USE_GRPC_SYNC_BARE)
    Base::ThreadExecutor* sync_client_task_queue = nullptr;
#endif

    mutable std::mutex mut;
    NuftCallbackFunc callbacks[NUFT_CB_SIZE];
    bool debugging = false;

    void print_state(std::lock_guard<std::mutex> & guard){
        printf("%15s %12s %5s %9s %7s %7s %6s %6s\n",
               "Name", "State", "Term", "log size", "commit", "peers", "run", "trans");
        printf("%15s %12s %5llu %9u %7lld %7u %6s %6d\n", name.c_str(),
               RaftNode::node_state_name(state), current_term,
               logs.size(), commit_index, peers.size(),
               is_running(guard)? "T" : "F", (!trans_conf) ? 0 : trans_conf->state);
    }

    NuftResult invoke_callback(Nuft_CB_Type type,NuftCallbackArg arg){
        if(callbacks[type]){
            return (callbacks[type])(type,&arg);
        }
        return Nuft_OK;
    }

    void set_callback(std::lock_guard<std::mutex>& guard, Nuft_CB_Type type,NuftCallbackFunc func){
        callbacks[type] = func;
    }

    void ste_callback(Nuft_CB_Type type,NuftCallbackFunc func){
        GUARD
        set_callback(guard,type,func);
    }

    IndexID get_base_index()const {
        if(logs.size()==0){
            return default_index_cursor;
        }
        return logs[0].index();
    }

    IndexID last_log_index()const {
        if(logs.size()==0){
            return default_index_cursor;
        }

        if(get_base_index()==0){
            assert(logs.size()==logs.back().index+1);
        }
        return logs.back().index();
    }

    IndexID last_log_term()const {
        return logs.size()==0?default_term_cursor:logs.back().term();
    }

    ::raft_message::LogEntry& gl(IndexID index){
        IndexID base = get_base_index();
        if(index-base>=0){
            return logs[index-base];
        }else {
            debug_node("Access index %lld\n",index);
            assert(false);
        }
    }
};
