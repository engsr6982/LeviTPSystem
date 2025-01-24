# LeviTPSystem 传送系统

```
LeviTPSystem/
├── src/
│   ├── core/                    # 核心功能
│   │   ├── database/           # 数据存储
│   │   │   ├── Storage.h       # 存储接口
│   │   │   ├── LevelDB.h       # LevelDB实现
│   │   │   └── Migration.h     # 数据迁移
│   │   ├── economy/            # 经济系统(整合前置)
│   │   └── permission/         # 权限系统(整合前置)
│   ├── features/               # 功能模块
│   │   ├── home/              # 家园传送
│   │   ├── warp/              # 公共传送
│   │   ├── tpa/               # 玩家传送
│   │   ├── tpr/               # 随机传送
│   │   └── death/             # 死亡相关
│   ├── common/                 # 公共组件
│   │   ├── models/            # 数据模型
│   │   ├── utils/             # 工具类
│   │   └── constants/         # 常量定义
│   └── api/                    # 对外接口
├── tests/                      # 单元测试
└── docs/                       # 文档
```