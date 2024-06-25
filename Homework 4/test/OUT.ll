; ModuleID = 'print2.ll'
source_filename = "printloop.c"
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

4:                                                ; preds = %13, %0
  %.02 = phi i32 [ 0, %0 ], [ %14, %13 ]
  %5 = icmp slt i32 %.02, 10
  br i1 %5, label %6, label %37

6:                                                ; preds = %4
  %7 = sext i32 %.02 to i64
  %8 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %7
  store i32 %.02, ptr %8, align 4
  %9 = sext i32 %.02 to i64
  %10 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %9
  %11 = load i32, ptr %10, align 4
  %12 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %11)
  br label %15

13:                                               ; preds = %26
  %14 = add nsw i32 %.02, 1
  br label %4, !llvm.loop !5

15:                                               ; preds = %6
  %16 = sext i32 %.02 to i64
  %17 = getelementptr inbounds [10 x i32], ptr %1, i64 0, i64 %16
  %18 = load i32, ptr %17, align 4
  %19 = add nsw i32 %18, 2
  %20 = sext i32 %.02 to i64
  %21 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %20
  store i32 %19, ptr %21, align 4
  %22 = sext i32 %.02 to i64
  %23 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %22
  %24 = load i32, ptr %23, align 4
  %25 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %24)
  br label %26

26:                                               ; preds = %15
  %27 = sext i32 %.02 to i64
  %28 = getelementptr inbounds [10 x i32], ptr %2, i64 0, i64 %27
  %29 = load i32, ptr %28, align 4
  %30 = mul nsw i32 %29, 3
  %31 = sext i32 %.02 to i64
  %32 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %31
  store i32 %30, ptr %32, align 4
  %33 = sext i32 %.02 to i64
  %34 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %33
  %35 = load i32, ptr %34, align 4
  %36 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %35)
  br label %13

37:                                               ; preds = %4
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
