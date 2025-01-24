# LeviTPSystem 传送系统

```
LeviTPSystem/
└── src/
    ├── core/               # 核心功能
    │   ├── database/         # 数据存储
    │   │   ├── Storage.h       # 存储接口
    │   │   └── Migration.h     # 数据迁移
    │   ├── economy/          # 经济系统
    │   └── permission/       # 权限系统
    ├── features/           # 功能模块
    │   ├── home/             # 家园传送
    │   │   ├── models/
    │   │   ├── event/
    │   │   └── ...
    │   ├── warp/             # 公共传送
    │   │   ├── models/
    │   │   ├── event/
    │   │   └── ...
    │   ├── tpa/              # 玩家传送
    │   │   ├── models/
    │   │   ├── event/
    │   │   └── ...
    │   ├── tpr/              # 随机传送
    │   │   ├── models/
    │   │   ├── event/
    │   │   └── ...
    │   └── death/            # 死亡相关
    │       ├── models/
    │       ├── event/
    │       └── ...
    └── common/             # 公共模块
        ├── utils/            # 工具类
        ├── config/           # 配置文件
        └── ...
```
