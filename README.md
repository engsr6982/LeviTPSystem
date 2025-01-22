# LeviTPSystem 传送系统

特性：

- 家园传送
- 公共传送
- 随机传送（TPR）
- 玩家传送（TPA）
- 死亡点传送
- 死亡点查询
- 命令/GUI 操作
- 玩家自定义规则

## 安装

```bash
lip install github.com/engsr6982/LeviTPSystem
```

## 命令系统

> LeviTPSystem 的命令系统是以`顶层命令+功能枚举+操作名+参数`组成  
> 这样可以避免多种同类型插件命令冲突（重复注册）

<details>
  <summary>命令详解 [点我展开]</summary>

> 注意: 插件默认注册的顶层命令为`tps`, 如有修改请将下文的`tps`换成你修改后的顶层命令

- 家 命令

`/tps home` 家园传送点 GUI（玩家）

`/tps home add <name: string>` 添加一个家（玩家）

`/tps home del <name: string>` 删除一个家（玩家）

`/tps home go <name: string>` 前往家（玩家）

`/tps home list` 列出所有家（玩家）

- 公共传送点命令

`/tps warp` 公共传送点 GUI（玩家）

`/tps warp add <name: string>` 添加一个公共传送点（权限组允许）（玩家）

`/tps warp del <name: string>` 删除一个公共传送点（权限组允许）（玩家）

`/tps warp go <name: string>` 前往公共传送点（玩家）

`/tps warp list` 列出所有公共传送点（玩家）

- Tpa 命令

`/tps tpa` 打开 Tpa GUI（玩家）

`/tps tpa accept` 接受一个 Tpa 请求（玩家）

`/tps tpa deny` 拒绝一个 Tpa 请求（玩家）

`/tps tpa here <player: target>` 发起 Tpa 将目标玩家传送到我这（玩家）

`/tps tpa to <player: target>` 发起 Tpa 传送到目标玩家（玩家）

- 数据库命令

`/tps leveldb del <键1> [键2]` 删除数据库指定键下的数据（控制台）

`/tps leveldb export` 导出当前数据库的所有数据（控制台）

`/tps leveldb import [旧数据模式: boolean]` 将 导出的数据/旧版本数据 导入数据库（控制台）

`/tps leveldb list [键1] [键2]` 列出所有键（控制台）

- 控制台命令

`/tps reload` 重载配置文件（控制台）

- 其他

`/tps` 和 `/tps menu`打开主菜单（玩家）

`/tps mgr` 打开管理 GUI（插件 OP）

`/tps back` 返回死亡点 GUI（玩家）

`/tps death` 查询死亡信息（玩家）

`/tps pr` 打开 Pr GUI（玩家）

`/tps rule` 打开规则配置（玩家）

`/tps tpr` 随机传送 GUI（玩家）

`/tps perm add <realName: string> <permtype: PermType>` 添加一个权限（控制台）

`/tps perm del <realName: string> <permtype: PermType>` 添加一个权限（控制台）

`/tps perm list <realName: string>` 列出玩家拥有的权限（控制台）

</details>

`PermType` 权限类型
- add_warp 添加公共传送点
- delete_warp 删除公共传送点
- edit_warp 编辑公共传送点(add_warp + delete_warp)
- manager_panel 管理 GUI
- home_count_unlimited 不限制家园数量

## 配置文件

- Config.json

> "bds\plugins\LeviTPSystem\Config.json"

```json
{
  "Command": {
    "Command": "tps", // 顶层命令
    "Description": "LeviTPSystem" // 命令描述
  },
  "EconomySystem": {
    "enabled": false, // 是否启用经济系统
    "kit": "LegacyMoney", // 经济套件，支持 LegacyMoney、Scoreboard
    "currency": "money", // 经济名称
    "scoreboard": "" // 计分板名称
  },
  "Tpa": {
    "Enable": true,
    "Money": 0, // Tpa 成功后扣除的经济
    "CacheExpirationTime": 120, // Tpa 请求缓存过期时间（秒）
    "CacehCheckFrequency": 60 // Tpa 请求缓存检查频率（秒）
  },
  "Home": {
    "Enable": true,
    "CreatHomeMoney": 0, // 创建家园花费的经济
    "GoHomeMoney": 0, // 前往家园花费的经济
    "EditHomeMoney": 0, // 编辑家园花费的经济
    "DeleteHomeMoney": 0, // 删除家园花费的经济
    "MaxHome": 20 // 最大家园数量
  },
  "Warp": {
    "Enable": true,
    "GoWarpMoney": 0 // 前往公共传送点花费的经济
  },
  "Death": {
    "Enable": true,
    "GoDeathMoney": 0, // 死亡传送花费的经济
    "MaxDeath": 5 // 死亡点最大记录数量
  },
  "Tpr": {
    "Enable": true,
    "Money": 0, // 随机传送花费的经济
    "RandomRangeMin": -1000, // 随机传送的最小范围
    "RandomRangeMax": 1000, // 随机传送的最大范围
    "Dimensions": [0, 1, 2], // 允许随机传送的维度
    "DangerousBlocks": ["minecraft:lava", "minecraft:flowing_lava"], // 危险方块列表，落脚点是这些方块则传送失败
    "RestrictedArea": {
      // 限制传送范围
      "Enable": false,
      "Type": "Circle", // 限制类型，支持 Circle、CenteredSquare
      "CenterX": 0, // 中心点X坐标
      "CenterZ": 0, // 中心点Z坐标
      "Radius": 100, // 半径
      "UsePlayerPos": false // 是否使用玩家当前位置作为限制中心(以玩家位置为中心进行随机传送)
    }
  },
  "Pr": {
    "Enable": true,
    "SendRequestMoney": 0, // 创建请求花费的经济
    "DeleteRequestMoney": 0 // 删除请求花费的经济
  },
  "Rule": {
    // 玩家自定义规则默认配置
    "deathPopup": true, // 死亡后弹出返回死亡点弹窗
    "allowTpa": true, // 玩家是否接受他人发起的 Tpa 请求
    "tpaPopup": true // Tpa 请求发起后是否弹出提示框
  },
  "logLevel": "Info", // 日志等级，支持 Off / Fatal / Error / Warn / Info / Debug / Trace
  "version": 2 // 配置文件版本号，除非你知道你在做什么，否则不要修改
}
```

- index.json

> "bds\plugins\LeviTPSystem\data\index.json"  
> 这是`/tps menu`表单的配置文件，支持子表单，类型请看 Ts 定义

```json
{
  "title": "LeviTPSystem Menu",
  "content": "选择一个功能：",
  "buttons": [
    {
      "title": "家园传送",
      "imageType": "path", // url 或 path
      "imageData": "textures/ui/village_hero_effect",
      "callbackType": "cmd", // 支持 cmd function subform
      "callbackRun": "tps home"
    },
    {
      "title": "公共传送",
      "imageType": "path",
      "imageData": "textures/ui/icon_best3",
      "callbackType": "function",
      "callbackRun": "warp"
    },
    {
      "title": "玩家传送",
      "imageType": "path",
      "imageData": "textures/ui/icon_multiplayer",
      "callbackType": "cmd",
      "callbackRun": "tps tpa"
    },
    {
      "title": "死亡传送",
      "imageType": "path",
      "imageData": "textures/ui/friend_glyph_desaturated",
      "callbackType": "cmd",
      "callbackRun": "tps back"
    },
    {
      "title": "随机传送",
      "imageType": "path",
      "imageData": "textures/ui/mashup_world",
      "callbackType": "cmd",
      "callbackRun": "tps tpr"
    },
    {
      "title": "个人设置",
      "imageType": "path",
      "imageData": "textures/ui/icon_setting",
      "callbackType": "cmd",
      "callbackRun": "tps rule"
    }
  ]
}
```

## 迁移指南

当前移植版本的`LeviTPSystem`与`LSE-TPSystem`的数据库通用  
但是为了保险起见，我们建议您在导入之前备份`leveldb`文件夹

注意：移植版的`Config.json`与旧版有些许差异，请根据需要修改
`data/index.json`的文件与旧版本不兼容，请不要使用旧版本的`formJSON.json`

1. 备份`leveldb`文件夹
2. 删除旧版`LSE-TPSystem`插件
3. 使用上面的安装命令，安装 LeviTPSystem
4. 启动服务器，等待插件生成新配置文件
5. 与旧的`Config.json`对比，修改新版的`Config.json`
6. 复制`leveldb`文件夹到新版的`bds\plugins\LeviTPSystem`目录下
7. 启动服务器即可

## 其他信息

### 目录结构

```floder
LeviTPSystem-x64-windows.zip
|   Config.json                 // 配置文件
|   LeviTPSystem_Debug.dll      // 插件本体
|   LeviTPSystem_Debug.pdb      // 插件调试信息
|   manifest.json               // 插件入口定义
|
+---data
|       index.json              // 菜单配置文件
|
+---export                      // 数据库导出文件夹
+---import                      // 数据库导入文件夹
+---lang                        // 语言文件夹（默认不存在）
\---leveldb                     // 数据库文件夹（与旧版本兼容）
```
