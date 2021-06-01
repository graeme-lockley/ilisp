(import "../../../unit.scm" :as Unit)

(import "./type.scm" :as Type)
(import "./module.scm" :as Module)

(Unit.test "identified-type->string"
    (Unit.assert-equals 
        (Module.identified-type->string (Module.IdentifiedType "%struct.Value" (Type.Structure #f (list Type.i32 (Type.Reference "%union.anon")))))
        "%struct.Value = type {i32, %union.anon}"
    )
)
