if ("true" -eq $env:CI_WINDOWS) {
    # Install Qt-static
    curl -LJO https://github.com/lkeegan/qt5-static/releases/download/5.14.2/qt5-static-win32.zip 
    7z x qt5-static-win32.zip
    $env:PATH = "$env:APPVEYOR_BUILD_FOLDER\install\bin;C:\go114\bin;C:\msys64\mingw64\bin;" + $env:PATH

    # Build Go
    cd ./src/cgoqt
    go build -buildmode=c-archive
    cd ../../

    # Build Qt
    mkdir build
    cd build
    qmake CONFIG+=release ../src/xmly-downloader-qt5.pro
    mingw32-make -j2

    mkdir xmly-downloader-qt5_windows
    Copy-Item .\release\xmly-downloader-qt5.exe .\xmly-downloader-qt5_windows
    7z a xmly-downloader-qt5_windows_v$env:APPVEYOR_BUILD_VERSION.zip .\xmly-downloader-qt5_windows
}
elseif ("true" -eq $env:CI_LINUX) {
    # Install library
    sudo apt-get install -y libglib2.0-dev
    sudo apt-get install -y libxkbcommon-dev
    
    # Install Qt-static
    mkdir qt5
    Set-Location qt5
    curl -LJO https://github.com/lkeegan/qt5-static/releases/download/5.14.2/qt5-static-linux.tgz
    tar xzf qt5-static-linux.tgz
    $env:PATH = "$env:APPVEYOR_BUILD_FOLDER/qt5/usr/local/qt5-static/bin:" + $env:PATH
    
    # Build Go
    cd ../src/cgoqt
    go build -buildmode=c-archive
    cd ../../
    
    # Build Qt
    mkdir build
    cd build
    qmake CONFIG+=release ../src/xmly-downloader-qt5.pro
    make -j2
    
    mkdir xmly-downloader-qt5_linux
    Copy-Item xmly-downloader-qt5 .\xmly-downloader-qt5_linux
    Write-Output '#！/bin/bash\n./xmly-downloader-qt5' > .\xmly-downloader-qt5_linux\xmly-downloader-qt5.sh
    tar zcvf xmly-downloader-qt5_linux_v$env:APPVEYOR_BUILD_VERSION.tar.gz xmly-downloader-qt5_linux
}
elseif ("true" -eq $env:CI_MACOS) {
    # Install Qt-static
    mkdir qt5
    Set-Location qt5
    curl -LJO https://github.com/lkeegan/qt5-static/releases/download/5.14.2/qt5-static-osx.tgz
    tar xzf qt5-static-osx.tgz
    $env:PATH = "$env:APPVEYOR_BUILD_FOLDER/qt5/usr/local/qt5-static/bin:" + $env:PATH

    # Build Go
    cd ../src/cgoqt
    go build -buildmode=c-archive
    cd ../../

    # Build Qt
    mkdir build
    Set-Location build
    qmake CONFIG+=release ../src/xmly-downloader-qt5.pro
    make -j2

    tar zcvf xmly-downloader-qt5_macos_v$env:APPVEYOR_BUILD_VERSION.tar.gz xmly-downloader-qt5.app
}
