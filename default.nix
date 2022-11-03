with import <nixpkgs> {};
stdenv.mkDerivation {
  src = ./.;
  name = "ff-overlay";
  enableParallelBuilding = true;

  installPhase = ''
      make install PREFIX=$out
    '';
}
