{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    nativeBuildInputs = with pkgs; [
      cmake
      doxygen
      gcc
      clang
      ninja
    ];

    buildInputs = with pkgs; [
      assimp
      glfw
      glm
    ];
  }
