; ModuleID = 'pl2r.ll'
source_filename = "printloop2.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-macosx14.0.0"

@.str = private unnamed_addr constant [7 x i8] c"I: %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [7 x i8] c"J: %d\0A\00", align 1
@.str.2 = private unnamed_addr constant [7 x i8] c"K: %d\0A\00", align 1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
  %1 = alloca [10 x i32], align 4
  %2 = alloca [10 x i32], align 4
  %3 = alloca [10 x i32], align 4
  br label %4

4:                                                ; preds = %14, %0
  %.04 = phi i32 [ 0, %0 ], [ %15, %14 ]
  %.02 = phi i32 [ 0, %0 ], [ %7, %14 ]
  %.03 = phi i32 [ 0, %0 ], [ %17, %14 ]
  %5 = icmp slt i32 %.04, 10
  br i1 %5, label %6, label %31

6:                                                ; preds = %4
  %7 = add nsw i32 %.02, 1
  %8 = sext i32 %.04 to i64
  %9 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %8
  store i32 %7, ptr %9, align 4
  %10 = sext i32 %.04 to i64
  %11 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %10
  %12 = load i32, ptr %11, align 4
  %13 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %12)
  br label %16

14:                                               ; preds = %24
  %15 = add nsw i32 %.04, 1
  br label %4, !llvm.loop !5

16:                                               ; preds = %6
  %17 = add nsw i32 %.03, 2
  %18 = sext i32 %.04 to i64
  %19 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %18
  store i32 %17, ptr %19, align 4
  %20 = sext i32 %.04 to i64
  %21 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %20
  %22 = load i32, ptr %21, align 4
  %23 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %22)
  br label %24

24:                                               ; preds = %16
  %25 = sext i32 %.04 to i64
  %26 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %25
  store i32 %.04, ptr %26, align 4
  %27 = sext i32 %.04 to i64
  %28 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %27
  %29 = load i32, ptr %28, align 4
  %30 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %29)
  br label %14

31:                                               ; preds = %4
  ret i32 0
}

declare i32 @printf(ptr noundef, ...) #1

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+crc,+dotprod,+fp-armv8,+fp16fml,+fullfp16,+lse,+neon,+ras,+rcpc,+rdm,+sha2,+sha3,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8.5a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"clang version 17.0.6"}
!5 = distinct !{!5, !6}
!6 = !{!"llvm.loop.mustprogress"}
