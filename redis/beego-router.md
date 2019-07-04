###  路由设置

- 基本路由设置

  - 基本GET路由

    ``` go
    beego.Get("/",func(ctx *context.Context){
         ctx.Output.Body([]byte("hello world"))
    })
    
    ```

  - 基本POST路由

    ``` go
    beego.Any("/foo",func(ctx *context.Context){
         ctx.Output.Body([]byte("bar"))
    })
    ```

  - 自定义响应任意HTTP路由,这个函数是关键,第一个参数表示路由 URI, 第二个就是你自己实现的 `http.Handler`, 注册之后就会把所有 rpc 作为前缀的请求分发到 `http.Handler` 中进行处理

    ``` go
    beego.Handler(router, http.Handler)
    ```

  - 目前支持

    ``` go
    beego.Get(router, beego.FilterFunc)
    beego.Post(router, beego.FilterFunc)
    beego.Put(router, beego.FilterFunc)
    beego.Patch(router, beego.FilterFunc)
    beego.Head(router, beego.FilterFunc)
    beego.Options(router, beego.FilterFunc)
    beego.Delete(router, beego.FilterFunc)
    beego.Any(router, beego.FilterFunc)
    ```

- 固定路由（全匹配路由）一个固定的路由，一个控制器，然后根据用户请求方法不同请求控制器中对应的方法

  ``` go
  beego.Router("/", &controllers.MainController{})
  beego.Router("/admin", &admin.UserController{})
  ```

- 正则路由（根据url路径进行正则化匹配）

  ``` go
  beego.Router(“/api/?:id”, &controllers.RController{})
  
  // 默认匹配 //例如对于URL”/api/123”可以匹配成功，此时变量”:id”值为”123”
  
  // 其中的id参数值可以取出
  this.Ctx.Input.Param(":id")
  ```

- 自定义方法及RESTful原则

  ``` go
  beego.Router("/",&IndexController{},"*:Index") // Index 对应contre
  beego.Router("/api",&RestController{},"get,post:ApiFunc")
  beego.Router("/simple",&SimpleController{},"get:GetFunc;post:PostFunc")
  ```

  

