# LeviTPSystem 传送系统

集成 Home, Warp, Tpa, Tpr, Death 等模块的传送系统

## 功能

- [x] Home - 家园传送
- [x] Warp - 公共传送点
- [x] Tpa - 玩家传送
- [x] Tpr - 随机传送
- [x] Death - 死亡点(传送、查询)
- [x] Setting - 玩家个性设置
- [x] GUI - 表单界面
- [x] i18n - 国际化

## 命令列表

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
/home                            # [玩家] GUI
/home add <name>                 # [玩家] 添加传送点 (当前位置)
/home remove <name>              # [玩家] 删除传送点
/home go <name>                  # [玩家] 传送 (传送点名称)
/home list [name]                # [玩家] 列出传送点
/home mgr                        # [管理] 管理员GUI

# Warp 模块
/warp                            # [玩家] GUI
/warp add <name>                 # [管理] 添加传送点 (当前位置)
/warp remove <name>              # [管理] 删除传送点
/warp go <name>                  # [玩家] 传送 (传送点名称)
/warp list [name]                # [玩家] 列出传送点
/warp mgr                        # [管理] 管理员GUI

# Tpa 模块 √
/tpa                             # [玩家] GUI
/tpa <accept|deny>               # [玩家] 接受|拒绝 传送请求
/tpa here <player: target>       # [玩家] 发起 Tpa 请求 (目标玩家传送到我)
/tpa to <player: target>         # [玩家] 发起 Tpa 请求 (我传送到目标玩家)

# Tpr 模块
/tpr                             # [玩家] GUI

# Death 模块
/death                           # [玩家] GUI
/death list [page]               # [玩家] 列出死亡点
/death back [id]                 # [玩家] 返回死亡点 (id 按死亡时间排序，默认从新到旧)

/back                            # [玩家] 返回死亡点 (最近一次，等价于 /death back 0)
```
