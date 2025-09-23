if [ ! -d "./build" ]; then
    mkdir build
fi

cd build

# Check and install SFML if not present
if ! dpkg -s libsfml-dev >/dev/null 2>&1; then
    echo "SFML not found. Installing SFML..."
    sudo apt-get update
    sudo apt-get install -y libsfml-dev
else
    echo "SFML is already installed."
fi


if [ ! -d "./lib" ]; then
    mkdir lib
fi

cd lib

if [ ! -d "./pkgconfig" ]; then
    mkdir pkgconfig
fi

cd ..

if [ ! -d "./include" ]; then
    mkdir include
fi

if [ ! -d "./bin" ]; then
    mkdir bin
fi

declare LOCAL_DEPS_BUILD_DIR
LOCAL_DEPS_BUILD_DIR=$(pwd)

if [ ! -d "./ticktoucan" ]; then
    mkdir ticktoucan
fi

cd ticktoucan

#setup ticktoucan
if [ -d "./ticktoucan" ]; then
    echo "ticktoucan directory already exists!!!"
elif [ -d "../../../ticktoucan" ]; then
    echo "local ticktoucan found ... copying ..."
    cp -r ../../../ticktoucan ./
else
    echo "Installing ticktoucan..."
    git clone https://github.com/narenr94/ticktoucan.git
fi

cd ./ticktoucan

bash ./install-sim.sh $LOCAL_DEPS_BUILD_DIR

cd ../../

if [ ! -d "./digitdodo" ]; then
    mkdir digitdodo
fi

cd digitdodo

if [ ! -d "./dd_platform" ]; then
    mkdir dd_platform
fi

cd dd_platform

PKG_CONFIG_PATH=${LOCAL_DEPS_BUILD_DIR}/lib/pkgconfig cmake ../../../sim/platform/ -DCMAKE_INSTALL_PREFIX=${LOCAL_DEPS_BUILD_DIR}

make
make install


echo -e 'prefix='$LOCAL_DEPS_BUILD_DIR'/lib \nexec_prefix='$LOCAL_DEPS_BUILD_DIR' \nlibdir='$LOCAL_DEPS_BUILD_DIR'/lib \nincludedir='$LOCAL_DEPS_BUILD_DIR'/include \n \nName: digitdodoplatform \nDescription: platform implementation of digit dodo \nVersion: 1.0 \nLibs: -L${libdir} -ldigitdodoplatform \nCflags: -I${includedir}' > $LOCAL_DEPS_BUILD_DIR/lib/pkgconfig/libdigitdodoplatform.pc

cd ..

if [ ! -d "./dd_middleware" ]; then
    mkdir dd_middleware
fi

cd dd_middleware

PKG_CONFIG_PATH=${LOCAL_DEPS_BUILD_DIR}/lib/pkgconfig cmake ../../../ -DCMAKE_INSTALL_PREFIX=${LOCAL_DEPS_BUILD_DIR}
make
make install

echo -e 'prefix='$LOCAL_DEPS_BUILD_DIR'/lib \nexec_prefix='$LOCAL_DEPS_BUILD_DIR' \nlibdir='$LOCAL_DEPS_BUILD_DIR'/lib \nincludedir='$LOCAL_DEPS_BUILD_DIR'/include \n \nName: digitdodo \nDescription: middleware implementation of digit dodo \nVersion: 1.0 \nLibs: -L${libdir} -ldigitdodo \nCflags: -I${includedir}' > $LOCAL_DEPS_BUILD_DIR/lib/pkgconfig/libdigitdodo.pc

cd ..

if [ ! -d "./dd_sim" ]; then
    mkdir dd_sim
fi

cd dd_sim

PKG_CONFIG_PATH=${LOCAL_DEPS_BUILD_DIR}/lib/pkgconfig cmake ../../../sim/ -DCMAKE_INSTALL_PREFIX=${LOCAL_DEPS_BUILD_DIR}
make
make install