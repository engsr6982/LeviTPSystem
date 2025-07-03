# TeleportSystem 传送系统

集成 Home, Warp, Tpa, Tpr, Death 等模块的传送系统

## 功能&特性

|  功能   |        描述        | 状态 |
| :-----: | :----------------: | :--: |
|  Home   |      家园传送      | 完工 |
|  Warp   |     公共传送点     | 完工 |
|   Tpa   |      玩家传送      | 完工 |
|   Tpr   |      随机传送      | 完工 |
|  Death  | 死亡点(传送、查询) | 完工 |
| Setting |    玩家个性设置    | 完工 |

> 其它特性

- i18n 国际化
- 权限管理
- 传送冷却
- 经济对接
- 表达式价格计算
- 模块化设计
- Home / Warp 管理员 GUI
- 事件驱动模型(大量事件导出)
- SDK 导出（完整 API）

## Command 命令

```bash
# 基础命令
/ltps version                    # [玩家] 版本
/ltps reload                     # [控制台] 重载配置文件
/ltps setting                    # [玩家] 玩家设置

# 权限管理
/ltps perm list <builtin|default>                             # [控制台] 列出 内置权限 / 默认权限
/ltps perm list player <realName>                             # [控制台] 列出玩家权限
/ltps perm <add|remove> default <permission>                  # [控制台] 添加或移除默认权限
/ltps perm <add|remove> player <realName> <permission>        # [控制台] 添加或移除玩家权限
/ltps perm batch <add|remove> default <permissions>           # [控制台] 批量添加或移除默认权限 (用'|'分隔)
/ltps perm batch <add|remove> player <realName> <permissions> # [控制台] 批量添加或移除玩家权限 (用'|'分隔)

# Home 模块
/home                              # [玩家] GUI
/home add <name>                   # [玩家] 添加传送点 (当前位置)
/home remove <name>                # [玩家] 删除传送点
/home go <name>                    # [玩家] 传送 (传送点名称)
/home list [name]                  # [玩家] 列出传送点
/home update name <name> [newName] # [玩家] 更新传送点名称
/home update position <name>       # [玩家] 更新传送点位置
/home mgr                          # [管理] 管理员GUI

# Warp 模块
/warp                              # [玩家] GUI
/warp add <name>                   # [管理] 添加传送点 (当前位置)
/warp remove <name>                # [管理] 删除传送点
/warp go <name>                    # [玩家] 传送 (传送点名称)
/warp list [name]                  # [玩家] 列出传送点
/warp mgr                          # [管理] 管理员GUI

# Tpa 模块 √
/tpa                               # [玩家] GUI
/tpa <accept|deny>                 # [玩家] 接受|拒绝 传送请求
/tpa here <player: target>         # [玩家] 发起 Tpa 请求 (目标玩家传送到我)
/tpa to <player: target>           # [玩家] 发起 Tpa 请求 (我传送到目标玩家)

# Tpr 模块
/tpr                               # [玩家] GUI

# Death 模块
/death                             # [玩家] GUI
/death list                        # [玩家] 列出死亡点
/death back [id]                   # [玩家] 返回死亡点 (id 按死亡时间排序，默认从新到旧)
/back                              # [玩家] 返回死亡点 (最近一次，等价于 /death back 0)
```

## Config 配置文件

```json
{
  "version": 10, // 配置文件版本(请勿修改)
  "economySystem": {
    "enabled": false, // 是否启用经济系统
    "kit": "LegacyMoney", // 经济套件 目前仅支持 LegacyMoney
    "scoreboardName": "Scoreboard", // Scoreboard 经济系统使用的计分板名称 (暂不支持)
    "economyName": "Coin" // 经济系统货币名称
  },
  "modules": {
    "tpa": {
      "enable": true, // 是否启用 Tpa 模块
      "createRequestCalculate": "random_num_range(10, 60)", // 创建请求价格
      "cooldownTime": 10, // 发起请求冷却时间(秒)
      "expirationTime": 120, // 请求过期时间(秒)
      "disallowedDimensions": [] // 禁用维度
    },
    "home": {
      "enable": true, // 是否启用 Home 模块
      "createHomeCalculate": "random_num_range(10, 188)", // 创建传送点价格 变量：count (玩家已创建传送点数量)
      "goHomeCalculate": "random_num_range(10, 188)", // 传送价格 变量：dimid (传送点所在维度ID)
      "nameLength": 20, // 传送点名称长度限制
      "maxHome": 20, // 传送点数量限制
      "cooldownTime": 10, // 传送冷却时间(秒)
      "disallowedDimensions": [] // 禁用维度
    },
    "warp": {
      "enable": true, // 是否启用 Warp 模块
      "cooldownTime": 10, // 传送冷却时间(秒)
      "goWarpCalculate": "random_num_range(10, 60)", // 传送价格 变量：dimid (传送点所在维度ID)
      "disallowedDimensions": [] // 禁用维度
    },
    "death": {
      "enable": true, // 是否启用 Death 模块
      "registerBackCommand": true, // 是否注册 /back 命令
      "goDeathCalculate": "random_num_range(10, 60)", // 传送价格 变量：dimid (传送点所在维度ID) index (第几个死亡点，从新到旧)
      "maxDeathInfos": 5, // 最大记录死亡点数量
      "disallowedDimensions": [] // 禁用的维度
    },
    "tpr": {
      "enable": true, // 是否启用 Tpr 模块
      "cooldownTime": 10, // 传送冷却时间(秒)
      "calculate": "random_num_range(40, 80)", // 传送价格
      "randomRange": {
        "min": -1000, // 随机传送范围
        "max": 1000
      },
      "dangerousBlocks": [
        "minecraft:water", // 危险方块
        "minecraft:lava",
        "minecraft:fire"
      ],
      "restrictedAreas": {
        // 限制传送区域(启用后randomRange无效)
        "enable": false,
        "isCircle": true, // 是否为圆形: true 中心圆, false 中心矩形 (玩家为中心)
        "center": {
          "x": 0, // 中心点坐标
          "z": 0,
          "radius": 100, // 半径或半边长
          "usePlayerPositionCenter": false // 是否使用玩家位置作为中心点
        }
      },
      "disallowedDimensions": [] // 禁用维度
    }
  }
}
```
