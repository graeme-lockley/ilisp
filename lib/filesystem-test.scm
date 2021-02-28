(import "./filesystem.scm" :as FS)

(import "./unit.scm" :as Unit)

(Unit.test "absolute-name-relative-to-file-name"
    (Unit.assert-equals (FS.absolute-name-relative-to-file-name "/home/ilisp/some-other-file-name.scm" "./lib/string-kata-test.scm") "/home/ilisp/lib/string-kata-test.scm")
    (Unit.assert-equals (FS.absolute-name-relative-to-file-name "/workspaces/ilisp/lib/interpreter/interpreter-test.scm" "../unit.scm") "/workspaces/ilisp/lib/unit.scm")
)
