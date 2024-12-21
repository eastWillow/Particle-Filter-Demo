# Particle Filter demo

# Use the RayLib to Demo this algorithm

- [x] Set Up the Development Environment
- [x] Add a Constant Position Target
- [x] Add two Landmark
- [x] Calculate the Distance Between a Landmark and a Target
    - [x] Draw the line
    - [x] Draw the Distance Value
    - [x] Simple Distance
- [x] Integrate ImGui to Control These Positions
- [ ] Add Mouse Position Tracking
    - [x] Target
    - [ ] Landmark
- [x] Add Distance Random Noise
- [x] Caluate the Simulation Result 
- [x] Particle Filter Algorithm
    - [x] add test conditional
- [ ] Add More tune parameter panel
    - [ ] Display move speed & direction (FPS is sample rate)
    - [ ] Tune move speed & direction standard deviation
    - [ ] Tune Predict sigma
- [ ] Add more landmark to demo Particle Filter

# Prerequirement

* vscode : press F5 to download lib and build
* vscode extension : [vscode-catch2-test-adapter](https://marketplace.visualstudio.com/items?itemName=matepek.vscode-catch2-test-adapter)

# Run C Code 

1. git pull
2. cd build
3. ./premake5 or ./premake5.exe or premake5.osx
4. cd ..
5. make
6. ./bin/Debug/Particle-Filter-Demo

# C(Raylib) Result

[![](https://img.youtube.com/vi/Ss2pqZDDYWw/hqdefault.jpg)](http://www.youtube.com/watch?v=Ss2pqZDDYWw "")

# Matlab_code SOP

1. initial.m
2. partical_filter.m

# Matlab Result

[![](http://img.youtube.com/vi/_iThyEJgUIU/0.jpg)](http://www.youtube.com/watch?v=_iThyEJgUIU "")

# Cite
reference:http://ros-developer.com/2019/04/10/parcticle-filter-explained-with-python-code-from-scratch/
