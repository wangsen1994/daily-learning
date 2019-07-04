//
// Created by 王森 on 2019-06-27.
//

#include <string>
#include <mutex>
#include <vector>
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
#define GUARD std::lock_guard<std::mutex>guard((Mutex));

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
};

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

TermID current_term = default_term_cursor;
Nodename vote_for = vote_for_none;

std::vector<raft_message::LogEntry> logs;
std::string leader_name;

struct Configuration* tran_conf = nullptr;
struct Persister* persister = nullptr;

IndexID commit_index = default_index_cursor;
IndexID last_applied = default_index_cursor;


Nodename name;
