{ pkgs ? import <nixpkgs> {} }:

rec {
  box86-base = { target ? pkgs, host ? pkgs, nativeBuildInputs ? [], cmakeFlags ? [], ... } @ args: target.stdenv.mkDerivation (
    builtins.removeAttrs args [ "target" "host" "nativeBuildInputs" "cmakeFlags" ] // {
    name = "box86";
    src = ./.;
    nativeBuildInputs = with host; [ ninja cmake python3 git ] ++ nativeBuildInputs;
    cmakeFlags = [ "-DCMAKE_BUILD_TYPE=RelWithDebInfo" ] ++ cmakeFlags;
  });

  # musl needs lots more wrapped to work properly
  # musl also does not have fts etc..
  # maybe if box86 had "no-libc-wrappings" build option, musl could
  # make more sense for running stuff that are statically linked
  # box86-musl32 = box86-base { target = pkgs.pkgsCross.musl32; cmakeFlags = [ "-DNOALIGN=ON" "-DLD80BITS=ON" ]; };

  box86-gnu32 = box86-base {
    target = pkgs.pkgsCross.gnu32;
    cmakeFlags = [ "-DNOALIGN=ON" "-DLD80BITS=ON" ];
    installPhase = ''
      ctest .
      cmake --install .
    '';
  };

  androidpkgs = import (fetchTarball "https://github.com/Cloudef/nixpkgs/archive/android-prebuilt.tar.gz") {};

  box86-armv7a-android = box86-base {
    host = androidpkgs;
    target = androidpkgs.pkgsCross.armv7a-android-prebuilt;
    cmakeFlags = [ "-DANDROID=ON" "-DNOALIGN=OFF" "-DLD80BITS=OFF" "-DARM_DYNAREC=ON" "-DNOLOADADDR=ON" ];
  };
}
