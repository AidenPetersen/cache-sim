let
  pkgs = import <nixpkgs> {};
in
  pkgs.mkShell {
    buildInputs = with pkgs; [
   	gcc
	valgrind
	gnumake
	qemu_full
	python3
    cmake
    ];
  }

