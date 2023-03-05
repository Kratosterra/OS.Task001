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
    int fd_input;
    int fd_output;
    if (argc != 3)
    {
        printf("Ошибка, задайте в аргументах 2 имени файла\n");
        exit(-1);
    }
    char *input_file = argv[1];
    char *output_file = argv[2];
    fd_input = open(input_file, O_RDONLY);
    if (fd_input < 0)
    {
        printf("Независимый процесс 1: Не удалось открыть файл с входными данными!\n");
        exit(-1);
    }
    fd_output = open(output_file, O_CREAT | O_WRONLY, 0777);
    if (fd_output < 0)
    {
        printf("Независимый процесс 1: Не удалось открыть/создать файл с выходными данными!\n");
        exit(-1);
    }
    mkfifo(input_to_task, 0666);
    fd_fifo = open(input_to_task, O_WRONLY);
    if (fd_fifo < 0)
    {
        printf("Независимый процесс 1: Не удалось открыть неименованный канал для записи!\n");
        exit(-1);
    }
    status = read(fd_input, input, buf);
    printf("Независимый процесс 1: Удалось получить данные из файла ввода (%d): %s\n", status, input);
    status = write(fd_fifo, input, buf);
    printf("Независимый процесс 1: Удалось записать входные данные для процесса 2 (%d): %s\n", status, input);
    close(fd_input);
    close(fd_fifo);

    fd_fifo = open(input_to_task, O_RDONLY);
    if (fd_fifo < 0)
    {
        printf("Независимый процесс 1: Не удалось открыть неименованный канал для чтения!\n");
        exit(-1);
    }

    status = read(fd_fifo, output, buf);
    printf("Независимый процесс 1: Удалось получить данные от 2 процесса (%d): %s\n", status, output);
    write(fd_output, output, status);
    printf("Независимый процесс 1: Удалось записать данные от 2 процесса в файл вывода (%d): %s\n", status, output);
    close(fd_output);
    close(fd_fifo);
    unlink(input_to_task);
    return 0;
}