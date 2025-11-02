{
  inputs = {
    toolchain = {
      url = "github:myclevorname/nix-calculators";
      inputs.flake-utils.follows = "flake-utils";
    };
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      toolchain,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachSystem [ "x86_64-linux" "x86_64-darwin" ] (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages.default = toolchain.legacyPackages.${system}.buildCEProgram {
          name = "crossbun";
          src = self;
        };
      }
    );
}
