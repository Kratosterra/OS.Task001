#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "../functions/function.c"

int buf = 5000;

int main(int argc, char *argv[])
{
    char input[5000];
    char output[5000];
    int status;
    char input_to_task[] = "input_to_task.fifo";
    int fd_fifo;
    if (argc != 3)
    {
        printf("Ошибка, задайте в аргументах 2 имени файла\n");
        exit(-1);
    }
    char *input_file = argv[1];
    char *output_file = argv[2];
    mkfifo(input_to_task, 0666);
    fd_fifo = open(input_to_task, O_RDONLY);
    if (fd_fifo < 0)
    {
        printf("Независимый процесс 2: Не удалось открыть неименованный канал для чтения!\n");
        exit(-1);
    }
    status = read(fd_fifo, input, buf);
    printf("Независимый процесс 2: Получены данные из первого процесса (%d): %s\n", status, input);
    status = capitalizeWords(input, output);
    printf("Независимый процесс 2: Произведено решение, ответ (%d): %s\n", status, output);
    close(fd_fifo);
    fd_fifo = open(input_to_task, O_WRONLY);
    if (fd_fifo < 0)
    {
        printf("Независимый процесс 2: Не удалось открыть неименованный канал для записи!\n");
        exit(-1);
    }
    write(fd_fifo, output, status);
    printf("Независимый процесс 2: Удалось записать данные в неименованный канал для первого процесса (%d): %s\n", status, output);
    close(fd_fifo);
    unlink(input_to_task);
    return 0;
}