#include <stdio.h>

int main(int argc, char* argv[]) {

  int n = 10;

  int second_last = 1;
  int last = 1;

  printf("%d\n", second_last);
  printf("%d\n", last);

  for (int i = 1; i <= n; i++) {
    int next = second_last + last;
    printf("%d\n", next);
    second_last = last;
    last = next;
  }

  return 0;
}
