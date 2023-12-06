{ stdenv
, lib
, version
, cmake
, SDL2
, SDL2_mixer
}:

stdenv.mkDerivation {
  inherit version;

  pname = "rotten";

  src = ./.;

  nativeBuildInputs = [
    cmake
  ];

  buildInputs = [
    SDL2
    SDL2_mixer
  ];

  installPhase = ''
    runHook preInstall

    mkdir -p $out/bin
    cp rotten $out/bin

    runHook postInstall
  '';

  meta = with lib; {
    description = "An enhanced sourceport of Rise of the Triad, based on SDL2";
    homepage = "https://github.com/erysdren/ROTTEN";
    platforms = platforms.linux;
    license = licenses.gpl3Plus;
  };
}
