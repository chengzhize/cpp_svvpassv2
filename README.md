# cppExploitV2 — MD5 爆破工具

MD5 哈希爆破器，使用多线程穷举破解自定义格式的 MD5 哈希。

## 哈希格式

工具针对的哈希格式为：

```
hash = MD5( MD5(password) + MD5("hugo") ).substr(8, 16)
```

即：密码的 MD5 与固定 salt `"hugo"` 的 MD5 拼接后，再做一次 MD5，取中间 16 位。

## 算法

- 基于 **n 进制计数器** 穷举所有字符组合
- 字符集：数字 `0-9`（可扩展为完整可打印字符）
- 多线程：按**首字符**拆分任务，每个线程枚举以 `asciis[id]` 开头的所有组合
- **提前终止**：某个线程找到密码后，立即通知其他线程退出，避免浪费 CPU
- **进度条**：监控线程每 15 秒刷新一次进度和预估剩余时间，开销趋近于零

## 文件

| 文件 | 说明 |
|------|------|
| `main.cpp` | 主程序：爆破入口 + 多线程调度 + 穷举核心 |
| `md5.cpp` / `md5.h` | MD5 类实现（JieweiWei） |
| `md5_origin.cpp` | 函数式 MD5 实现（备选） |
| `gen.cpp` | 快速验证工具 |

## 编译

```bash
g++ main.cpp -o exploit.exe -std=c++11 -O2 -lpthread
```

## 使用

支持命令行参数和交互式输入两种方式。

### 命令行参数

```
-h, --hash <hash>      目标哈希（缺省则交互式询问）
-l, --length <length>  密码长度（缺省则交互式询问）
-o, --output <file>    结果写入文件（缺省则打印到控制台）
```

### 示例

```bash
# 完整参数，结果写入文件
./exploit.exe -h a1b2c3d4e5f6a1b2c3d4e5f6a1b2c3d4 -l 4 -o result.txt

# 长格式
./exploit.exe --hash a1b2c3d4e5f6a1b2c3d4 --length 4

# 混合：部分参数命令行，缺省的交互式输入
./exploit.exe -h a1b2c3d4e5f6a1b2c3d4e5f6a1b2c3d4
hash: a1b2c3d4e5f6a1b2c3d4e5f6a1b2c3d4
length: 4

# 纯交互式
./exploit.exe
hash: a1b2c3d4e5f6a1b2c3d4e5f6a1b2c3d4
length: 4
```

### 进度条

运行时进度条示例（每 15 秒刷新）：

```
Progress: [##############      ]  70%  12m34s  70000000/100000000
```

格式：`进度条 | 百分比 | 预估剩余时间 | 已完成/总计`

## 关于

暴力穷举 MD5，适合短密码（≤6 位数字）的快速恢复。字符集可扩展，多线程利用多核 CPU 加速。
