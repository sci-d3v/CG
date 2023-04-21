#!/bin/bash

#install extension
#https://milicendev.netlify.app/article/install-vs-codium-and-integrate-vs-code-extensions/
#https://marketplace.visualstudio.com/vscode

snap run codium --install-extension vsix/circledev.glsl-canvas-0.2.15.vsix
snap run codium --install-extension vsix/ms-vscode.cpptools-1.13.0@linux-x64.vsix
snap run codium --install-extension vsix/reloadedextensions.reloaded-cpp-0.1.9.vsix
snap run codium --install-extension vsix/hars.CppSnippets-0.0.15.vsix
snap run codium --install-extension vsix/ms-vscode.cpptools-extension-pack-1.3.0.vsix
snap run codium --install-extension vsix/slevesque.shader-1.1.5.vsix

snap run codium CG_P2