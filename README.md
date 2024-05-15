# LeviBoom

`LeviBoom` 名称的由来：  
"Levi" 是 "LeviLamina" 的前缀，而 "Boom"嘛：  
大家都喜欢在 Nas 上玩 All-in-one，但是 All-in-one 可能变成 All-in-Boom，所以就有了这个名字。

## 开发指南

### 自助构建

```bash
xmake f -a x64 -m debug -p windows -y --plugin=TPSystem
xmake
```

### 命名空间

```cpp
lbm                     // 全局命名空间
lbm::utils              // 全局工具类
lbm::modules            // 全局模块
lbm::library            // 全局库
lbm::api                // 全局 API（对 MCAPI 的封装）
lbm::entry              // 入口模块（对接 Levilamina）
lbm::utils::cmdtools    // 全局命令封装工具API
lbm::utils::mc          // 全局发送文本消息API封装
lbm::plugin             // 子插件全局命名空间
lbm::plugin::xxx        // 子插件 xxx 的专属命名空间
```

### 目录结构

```
├── plugin
│   └── tpsystem
│       └── Main.h  // 子插件入口头文件
├── src
│   └── ...         // 全局工具类、模块、库、API、入口模块源码
└── xmake.lua       // xmake 配置文件
```
