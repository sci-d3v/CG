#!/bin/bash

#https://snapcraft.io/install/codium/debian
sudo apt update
sudo apt install snapd
sudo snap install core

sudo snap install codium --classic

#install extension
#https://milicendev.netlify.app/article/install-vs-codium-and-integrate-vs-code-extensions/
#https://marketplace.visualstudio.com/vscode
chmod +x install_vsix.sh
