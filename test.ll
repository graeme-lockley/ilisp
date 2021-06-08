; ModuleID = 'test'

%struct.Value = type {i32, %union.anon}

%union.anon = type {i32*}

declare void @_print_value(%struct.Value*)

declare void @_print_newline()

declare %struct.Value* @_from_literal_string(i8*)

@.str0 = private unnamed_addr constant [12 x i8] [i8 104, i8 101, i8 108, i8 108, i8 111, i8 32, i8 119, i8 111, i8 114, i8 108, i8 100, i8 0], align 1

define i32 @main() {
  %1 = call %struct.Value* @_from_literal_string(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str0, i64 0, i64 0))
  call void @_print_value(%struct.Value* %1)
  call void @_print_newline()
  ret i32 0
}
