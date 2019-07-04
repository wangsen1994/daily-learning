## go learning

#### 匿名函数（闭包）

- 当用作递归函数时，需要首先声明一个变量，再将匿名函数复制给这个变量，若不分成两部分，则函数字面量无法与visitAll绑定

  ```go
  var visitAll func(items []string)
      visitAll = func(items []string) {
        							...
                  visitAll(m[item])
              }
          }
      }
  ```

- 通过数组对闭包进行返回时，注意将内部数据与各个闭包函数之间相互隔离（即是函数值中记录的是循环变量的地址，而不是某一时刻的值）

  ```go
  func main() {
      a := F()
      a[0]() //0xc00004c080 0
      a[1]() //0xc00004c088 1
      a[2]() //0xc00004c090 2
  }
  func F() []func() {
      b := make([]func(), 3, 3)
      for i := 0; i < 3; i++ {
          j := i
          b[i] = func() {
              fmt.Println(&j, j)
          }
      }
      return b
  }
  // 下面代码没有进行有效的隔离，容易出现bug
  func main() {
     a := F()
     a[0]()//0xc00004c080 3
     a[1]()//0xc00004c080 3
     a[2]()//0xc00004c080 3
  }
  func F() []func() {
     b := make([]func(), 3, 3)
     for i := 0; i < 3; i++ {
        b[i] = func() {
           fmt.Println(&i,i)
        }
     }
     return b
  }
  ```

#### 可变参数

- 在声明可变参数时，需要在参数列表的最后一个参数类型之前加上省略符号“...”，这表示该函数会接收任意数量的该类型参数。

  ``` go
  func sum(vals...int) int {
      total := 0
      for _, val := range vals {
          total += val
      }
      return total
  }
  ```

- 看似是将可变参数当作slice切片来进行处理，但实际其与传入的是slice是不同的

  ```go
  func f(...int) {}
  func g([]int) {}
  fmt.Printf("%T\n", f) // "func(...int)"
  fmt.Printf("%T\n", g) // "func([]int)"
  ```

- 若需要传入slice，则使用

  ``` go
  values := []int{1, 2, 3, 4}
  fmt.Println(sum(values...)) // "10"
  ```

- 接受任意类型参数

  ```go
  func errorf(linenum int, format string, args ...interface{})
  ```

#### Defferred函数

defer语句主要是用来延迟执行函数的，而且延迟发生在调用函数return之后，则其有如下几个用途，多个defer的执行顺序是先进后出，采用栈的处理方式。

- 清理释放资源（释放文件描述符，关闭锁等需要清理的操作）

- 被defer的函数可以读取和修改带名称的返回值

  ```go
  func c() (i int) {
      defer func() { i++ }()
      return 1
  } // c的返回值为2
  ```

- 利用defer实现对程序运行开始与结束的监控

  ```go
  func bigSlowOperation() {
      defer trace("bigSlowOperation")() // don't forget the
      extra parentheses
      // ...lots of work…
      time.Sleep(10 * time.Second) // simulate slow
      operation by sleeping
  }
  func trace(msg string) func() {
      start := time.Now()
      log.Printf("enter %s", msg)
      return func() { 
          log.Printf("exit %s (%s)", msg,time.Since(start)) 
      }
  }
  ```

- defer与recover合用实现对异常的处理，panic（会导致程序崩溃并输出日志信息，包括panic value和函数调用的堆栈跟踪信息）引发异常后会不断向上层抛出异常，直到遇到defer调用recover对异常进行捕获，为避免recover对所有的异常都进行恢复，可以通过对panic value进行对比，选择性的恢复异常。

####  方法

- 方法声明(附加的参数p，叫作方法的接收器，类似于java与C++中的self与this，常用对象的头字母来表示)，对底层类型是指针和interface的命名类型不能定义方法（不允许类型名本身是一个指针，若需要采用指针对象必须下面的方法定义）。

  ```go
  type Point struct{ X, Y float64 }
  
  // traditional function
  func Distance(p, q Point) float64 {
      return math.Hypot(q.X-p.X, q.Y-p.Y)
  }
  // same thing, but as a method of the Point type
  func (p Point) Distance(q Point) float64 {
      return math.Hypot(q.X-p.X, q.Y-p.Y)
  }
  ```

  

- 基于指针对象的方法，对该方法的调用会修改对象的值，而对于非指针对象的方法，即使你对象是指针，调用非指针对象的方法也不会更改对象的值。

  ```go
  type T struct {
      Name string
  }
  
  func (t T) M1() {
      t.Name = "name1"
  }
  
  func (t *T) M2() {
      t.Name = "name2"
  }
  
  t1 := T{"t1"}
  
  fmt.Println("M1调用前：", t1.Name) //t1
  t1.M1()
  fmt.Println("M1调用后：", t1.Name) //t1
  
  fmt.Println("M2调用前：", t1.Name) //t1
  t1.M2()// 这中间存在一个隐式的转换，所以如果不能取地址的变量是不能这样调用的
  fmt.Println("M2调用后：", t1.Name) //name2
  
  
  t2 := &T{"t2"}
  
  fmt.Println("M1调用前：", t2.Name) //t2
  t2.M1() // 这中间存在一个隐式的解引用过程
  fmt.Println("M1调用后：", t2.Name) //t2
  
  fmt.Println("M2调用前：", t2.Name) //t2
  t2.M2()
  fmt.Println("M2调用后：", t2.Name) //name2
  
  //嵌套情况
  
  type S struct {
      T
  }
  
  t1 := T{"t1"}
  s := S{t1}
  
  fmt.Println("M1调用前：", s.Name) //t1
  s.M1()
  fmt.Println("M1调用后：", s.Name) //t1
  
  fmt.Println("M2调用前：", s.Name) //t1
  s.M2()
  fmt.Println("M2调用后：", s.Name) //name2
  
  fmt.Println(t1.Name) //t1
  
  
  type S struct {
      *T
  }
  t1 := T{"t1"}
  s := S{&t1}
  
  fmt.Println("M1调用前：", s.Name) //t1
  s.M1()
  fmt.Println("M1调用后：", s.Name) //t1
  
  fmt.Println("M2调用前：", s.Name) //t1
  s.M2()
  fmt.Println("M2调用后：", s.Name) //name2
  
  fmt.Println(t1.Name) //name2
  ```

  

- Nil也是一个合法的接受类型

  ```go
  type IntList struct {
      Value int
      Tail  *IntList
  }
  // Sum returns the sum of the list elements.
  func (list *IntList) Sum() int {
      if list == nil {
          return 0
      }
      return list.Value + list.Tail.Sum()
  }
  ```

- 在类型中嵌套的匿名字段也可能是一个命名字段的指针，也可以有多个匿名字段，这种嵌套类型的方式，在调用方法时，编译器会首先在最外层类型的方法中去查找，然后在包含的字段中查找，所以不能在同一级拥有同名的函数，这样存在二义性。

#### 方法值和方法表达式

- 方法值：即是通过类型对象来选择其自身方法来赋值给特定接收器变量，其调用时只需传入函数参数即可，不需要制定接收器（类似于包含特定数据的函数对象）。

  ```go
  distanceFromP := p.Distance        // method value
  fmt.Println(distanceFromP(q))  
  ```

- 方法表达式与方法值类似，只是其没有类型对象来选择方法，所以调用方法表达式时需要传递类型对象与函数参数

  ``` go
  distance := Point.Distance   // method expression
  fmt.Println(distance(p, q))
  ```

- 封装 类型对象与方法（首字母大写，最小范围为package）

#### 接口（描述一系列方法的集合）

接口的定义类似于C++中的基函数，实现了基本调用方法的接口（也可以理解为C++ bind绑定函数接口，go就是利用这种方式来实现自定义格式内容的输出）。

```go
package io
// Writer is the interface that wraps the basic Write method.
type Writer interface {
    Write(p []byte) (n int, err error)
}
```

- 接口类型（可嵌套）

```go
package io
type Reader interface {
    Read(p []byte) (n int, err error)
}
type Closer interface {
    Close() error
}
// 内嵌
type ReadWriter interface {
    Reader
    Writer
}
```

- 对于命名过的具体类型T，它的一些方法的接受者可以是类型T或者是T指针，但是对于这些方法的调用必须是一个参数，不管是T类型或者T指针。

  ``` go
  type IntSet struct { /* ... */ }
  func (*IntSet) String() string
  var _ = IntSet{}.String() // compile error: String requires *IntSet receiver
  
  var s IntSet
  var _ = s.String() // OK: s is a variable and &s has a String method
  ```

- 只要一个类型的接受者方法和接口中定义的方法一致，就认为类型实现了接口。接口也属于一种类型，所以存在接口值的说法，其由两个部分组成，一个具体的类型和该类型的值（其概念有点类似于C++中的基类与派生类，常用于基类作为函数参数，接受派生类作为输入，但有别于C++的是，其基类形参完全继承派生类的数据值，所以其是一个动态类型和动态值）
- 警告：一个包含nil指针的接口不是nil接口（包含nil指针的接口其方法是可以被调用的，只是其接受者的值是nil）注意其调用的方法是否具有非空的隐含先决条件。
- 类型断言 x.(T)，x表示接口的类型，T表示具体类型，用于检查操作对象的动态类型是否与断言的类型匹配。有两种可能，第一种断言的类型T是一个具体类型，然后类型断言检查x的动态类型是否和T相同，如果成功，断言的结果是x的动态值，换句话说，具体类型的类型断言从它的操作对象中获取具体的值，如果检查失败，接下来这个操作会抛出panic；第二种如果相反断言的类型T是一个接口类型然后类型断言检查是否x的动态类型满足T，如果成功，这个结果仍然是一个相同类型和值部分的接口值，但结果有类型T。换句话说，对一个接口类型的类型断言改变了类型的表述方式，改变了可以获取的方法集合，但是他保护了接口值内部的动态类型和值部分。（没理解清楚这个东西）

#### Goroutines&Channel

除了从主函数退出或直接终止程序之外，没有其他编程方式能够让一个goroutine来打断另一个执行。

- 无缓冲channel	将导致发送者出现阻塞，直到接受者调用接受发送信息（即同步操作）。

- 级联的channel，这样可以避免各国routine不知道啥时间应该，重复关闭channel或关闭nil channel会引发panic。

  ``` go
  func main() {
      naturals := make(chan int)
      squares := make(chan int)
  
      // Counter
      go func() {
          for x := 0; x < 100; x++ {
              naturals <- x
          }
          close(naturals)
      }()
  
      // Squarer
      go func() {
          for x := range naturals {
              squares <- x * x
          }
          close(squares)
      }()
  
      // Printer (in main goroutine)
      for x := range squares {
          fmt.Println(x)
      }
  }
  ```

- 单向channel，下面存在channel的隐私转换，双向 chan int 与单向chan<-int（输出）、单向<- chan int（输入）之间的隐式转换。

  ```go
  func counter(out chan<- int) {
      for x := 0; x < 100; x++ {
          out <- x
      }
      close(out)
  }
  
  func squarer(out chan<- int, in <-chan int) {
      for v := range in {
          out <- v * v
      }
      close(out)
  }
  
  func printer(in <-chan int) {
      for v := range in {
          fmt.Println(v)
      }
  }
  
  func main() {
      naturals := make(chan int)
      squares := make(chan int)
      go counter(naturals)
      go squarer(squares, naturals)
      printer(squares)
  }
  ```

- 带缓冲channel（解耦合，并不需要同步）

  

#### 基于共享变量的并发

- 竞态条件 解决办法：1、固定访问数据  2、避免多个goroutine访问变量，通过channel传送到某固定goroutine来串行访问或更新  3、互斥加锁