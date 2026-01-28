{
  description = "Rust ARM embedded devshell";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    fenix.url = "github:nix-community/fenix";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    fenix,
    ...
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [fenix.overlays.default];
        };
        lib = pkgs.lib;

        target = "thumbv7em-none-eabihf";
        tomlToolchain = fenix.packages.${system}.fromToolchainFile {
          file = ./rust-toolchain.toml;
          sha256 = "sha256-KyNTI/ZRO/v6w+nJTxj8JjRMX4EmViw2pCTbRKYyILo=";
        };

        stableToolchain = with fenix.packages.${system};
          combine
          [
            complete.clippy
            complete.llvm-tools-preview
            complete.rust-analyzer
            targets.${target}.latest.rust-std
          ];
        rustToolchain = fenix.packages.${system}.combine [tomlToolchain stableToolchain];
        libs = with pkgs; [
          stdenv.cc.cc.lib
          zlib
        ];
      in {
        devShells.default = pkgs.mkShell {
          nativeBuildInputs = with pkgs;
            [
              uv
              (python312.withPackages (pypkgs:
                with pypkgs; [
                  numpy
                  # Not really needed, but some scripts in tflite-micro want it
                  pillow
                ]))

              gcc-arm-embedded
              cmake
              rustToolchain
              probe-rs-tools
              cargo-bloat
              cargo-binutils
            ]
            ++ (with python312Packages; [
              numpy
              uv
            ]);
          LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath libs}:";
        };
      }
    );
}
