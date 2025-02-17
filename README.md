# LD001

Uses Fat Controller for Simple Land Vehicle

# BUILD

```
mkdir build
cd build
cmake -S .. -B . 
cmake --build . --target install/strip
cpack -G DEB 
```

# INSTALLING

```
dpkg -i ./ld001_0.0.1_arm64.deb
systemctl start fatcnt
```
