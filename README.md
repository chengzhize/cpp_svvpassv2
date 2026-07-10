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

## 文件

| 文件 | 说明 |
|------|------|
| `main.cpp` | 主程序：爆破入口 + 多线程调度 + 穷举核心 |
| `md5.cpp` / `md5.h` | MD5 类实现（JieweiWei） |
| `md5_origin.cpp` | 函数式 MD5 实现（备选） |
| `gen.cpp` | 快速验证工具 |

## 编译

```bash
g++ main.cpp md5.cpp -o exploit.exe
```

## 使用

```bash
./exploit.exe
# 输入目标 hash → 输入密码长度 → 开始爆破
```

例：

```
hash: a1b2c3d4e5f6a1b2c3d4e5f6a1b2c3d4
length: 4
```

## 关于

暴力穷举 MD5，适合短密码（≤6 位数字）的快速恢复。字符集可扩展，多线程利用多核 CPU 加速。
