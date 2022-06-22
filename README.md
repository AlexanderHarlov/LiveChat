# LiveChat

### Prerequisites

* GCC
```
sudo apt install build-essential
```
* Hiredis - https://github.com/redis/hiredis.git
```
git clone https://github.com/redis/hiredis.git

cd hiredis

make

make install
```
* Redis plus plus - https://github.com/sewenew/redis-plus-plus.git
```
git clone https://github.com/sewenew/redis-plus-plus.git

cd redis-plus-plus

mkdir build

cd build

cmake -DREDIS_PLUS_PLUS_CXX_STANDARD=17 ..

make

make install

cd ..
```

### Compilation
For example project is located in /home/alex/Workspace/LiveChat

```
/usr/bin/g++ --std c++17 -fdiagnostics-color=always -I/home/alex/Workspace/LiveChat/views -g /home/alex/Workspace/LiveChat/*.cpp /home/alex/Workspace/LiveChat/views/src/*.cpp -o /home/alex/Workspace/LiveChat/app -lredis++ -lhiredis -lhiredis -lpthread
```
