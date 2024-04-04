define void @example_function(i32* %a, i32* %b, i32* %c, i32* %d, i32* %e, i32* %f, i32* %g, i32* %h, i32* %i, i32* %l, i32* %m, i32* %n, i32* %o) {
entry:
  ; a = a + 0
  %0 = load i32, i32* %a
  %1 = add i32 %0, 0
  store i32 %1, i32* %a

  ; b = 1 * b
  %2 = load i32, i32* %b
  %3 = mul i32 1, %2
  store i32 %3, i32* %b

  ; c = c * 8
  %4 = load i32, i32* %c
  %5 = mul i32 %4, 8
  store i32 %5, i32* %c

  ; d = d / 16
  %6 = load i32, i32* %d
  %7 = sdiv i32 %6, 16
  store i32 %7, i32* %d

  ; e = e - 0
  %8 = load i32, i32* %e
  %9 = sub i32 %8, 0
  store i32 %9, i32* %e

  ; f = g + 3
  %10 = load i32, i32* %g
  %11 = add i32 %10, 3
  store i32 %11, i32* %f

  ; h = f - 3
  %12 = sub i32 %11, 3
  store i32 %12, i32* %h

  ; i = l - 1
  %13 = load i32, i32* %l
  %14 = sub i32 %13, 1
  store i32 %14, i32* %i

  ; m = i + 1
  %15 = add i32 %14, 1
  store i32 %15, i32* %m

  ; n = n * 15
  store i32 3, i32* %n
  %16 = load i32, i32* %n
  %17 = mul i32 %16, 15
  store i32 %17, i32* %n

  ; o = 18 * o
  store i32 10, i32* %o
  %18 = load i32, i32* %o
  %19 = mul i32 18, %18
  store i32 %19, i32* %o

  ret void
}


