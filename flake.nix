{
  description = "Dev shell + runner for QMK c2json and QGF conversion";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    # Pin a qmk_firmware checkout so QMK_HOME is always valid.
    # qmk-firmware.url = "github:qmk/qmk_firmware";
    qmk-firmware = {
      url = "git+https://github.com/qmk/qmk_firmware?submodules=1&shallow=1";
      flake = false;
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      qmk-firmware,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
        cc = if pkgs.stdenv.hostPlatform.isDarwin then pkgs.clang else pkgs.gcc;

        qmkHome = qmk-firmware.outPath;

        # Wrapper ensures QMK_HOME is set for all invocations.
        qmkWrapped = pkgs.writeShellScriptBin "qmk" ''
          export QMK_HOME="${qmkHome}"
          exec ${pkgs.qmk}/bin/qmk "$@"
        '';
      in
      {
        # nix run . -- <qmk args>
        apps.default = {
          type = "app";
          program = "${qmkWrapped}/bin/qmk";
        };

        # nix develop
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            just
            qmk
            qmkWrapped
            cc # C preprocessor for c2json
            imagemagick # handy for pre-processing images
            python3Packages.pillow # safety net; qmk pulls it anyway
            keymap-drawer
          ];

          shellHook = ''
            export QMK_HOME="${qmkHome}"
            export QMK_USERSPACE="$PWD"
            echo "QMK_HOME=${qmkHome}"
            echo "QMK_USERSPACE=$QMK_USERSPACE"
            echo "qmk ready. Try: qmk --help"
          '';
        };

        # nix build .  -> builds the wrapper
        packages.default = qmkWrapped;
      }
    );
}
