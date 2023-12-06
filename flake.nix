{
  description = "An enhanced sourceport of Rise of the Triad, based on SDL2";

  outputs = { self, nixpkgs, ... }: let
    forAllSystems = function:
        nixpkgs.lib.genAttrs [
          "x86_64-linux"
        ] (system: function nixpkgs.legacyPackages.${system});
  in {
    packages = forAllSystems (pkgs: rec {
      rotten = pkgs.callPackage ./default.nix {
        version = self.lastModifiedDate;
      };

      default = rotten;
    });
  };
}
