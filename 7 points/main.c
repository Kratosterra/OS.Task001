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
    pid_t child_id_input, child_id_task;
    char input_to_task[] = "input_to_task.fifo";
    char task_to_output[] = "task_to_output.fifo";
    int input_task_fd;
    int task_output_fd;
    int fd_input;
    int fd_output;
    if (argc != 3)
    {
        printf("Ошибка, задайте в аргументах 2 имени файла");
        exit(-1);
    }
    char *input_file = argv[1];
    char *output_file = argv[2];
    (void)umask(0);
    mkfifo(input_to_task, S_IFIFO | 0666);
    mkfifo(task_to_output, S_IFIFO | 0666);

    child_id_input = fork();
    if (child_id_input < 0)
    {
        printf("Не удалось создать процесс ввода.\n");
        exit(-1);
    }
    if (child_id_input != 0)
    {
        printf("Cоздан процесс ввода!\n");
        child_id_task = fork();
        if (child_id_task < 0)
        {
            printf("Не удалось создать процесс решения.\n");
            exit(-1);
        }
        if (child_id_input != 0 && child_id_task != 0)
        {
            printf("Cоздан процесс выполнения задачи!\n");
        }
        else
        {
            printf("Процесс решения: Приступаем к чтению из именованного канала!\n");
            input_task_fd = open(input_to_task, O_RDONLY);
            // if (input_task_fd = open(input_to_task, O_RDONLY) < 0)
            //{
            //     printf("Процесс решения: Не могу открыть именованный канал для чтения!\n");
            //     exit(-1);
            // }
            status = read(input_task_fd, input, buf);
            printf("Процесс решения: удалось получить данные (%d): %s\n", status, input);
            if (close(input_task_fd) < 0)
            {
                printf("Процесс решения: Не могу закрыть именованный канал для чтения!\n");
                exit(-1);
            }
            int size_of_output = capitalizeWords(input, output);
            printf("Процесс решения: Завершил решение. Ответ: %s!\n", output);
            printf("Процесс решения: Приступаю к записи в новый именованный канал для записи ответа\n");
            task_output_fd = open(task_to_output, O_WRONLY);
            // if (task_output_fd = open(task_to_output, O_WRONLY) < 0)
            //{
            //     printf("Процесс ввода: Не могу открыть именованный канал для записи!\n");
            //     exit(-1);
            // }
            status = write(task_output_fd, output, size_of_output);
            printf("Процесс решения: удалось записать данные (%d): %s\n", status, output);
            if (close(task_output_fd) < 0)
            {
                printf("Процесс решения: Не могу закрыть именованный канал для записи!\n");
                exit(-1);
            }
            printf("Процесс решения: Завершил запись!\n");
        }
    }
    else
    {
        printf("Процесс ввода/вывода (ввод): Процесс ввода начинает считывание из файла %s!\n", input_file);
        fd_input = open(input_file, O_RDONLY);
        status = read(fd_input, input, buf);
        if (status < 0)
        {
            printf("Процесс ввода/вывода (ввод): Не удалось считать входные данные из %s!\n", input_file);
            exit(-1);
        }
        printf("Процесс ввода/вывода (ввод): удалось получить данные (%d): %s\n", status, input);
        printf("Процесс ввода/вывода (ввод): Приступаем к записи в именованный канал!\n");
        input_task_fd = open(input_to_task, O_WRONLY);
        // if (input_task_fd = open(input_to_task, O_WRONLY) < 0)
        //{
        //     printf("Процесс ввода: Не могу открыть именованный канал для записи!\n");
        //    exit(-1);
        //}
        status = write(input_task_fd, input, buf);
        printf("Процесс ввода/вывода (ввод): удалось записать данные (%d): %s\n", status, input);
        if (close(input_task_fd) < 0)
        {
            printf("Процесс ввода/вывода (ввод): Не могу закрыть именованный канал!\n");
            exit(-1);
        }
        printf("Процесс ввода/вывода (ввод): Завершил запись!\n");
        close(fd_input);
        printf("Процесс ввода/вывода (запись): Приступаем к чтению из именованного канала!\n");
        task_output_fd = open(task_to_output, O_RDONLY);
        // if (task_output_fd = open(task_to_output, O_RDONLY) < 0)
        //{
        //     printf("Процесс записи: Не могу открыть именованный канал для чтения!\n");
        //     exit(-1);
        // }
        status = read(task_output_fd, output, buf);
        printf("Процесс ввода/вывода (запись): удалось получить данные (%d): %s\n", status, output);
        if (close(task_output_fd) < 0)
        {
            printf("Процесс ввода/вывода (запись): Не могу закрыть именованный канал для чтения!\n");
            exit(-1);
        }
        fd_output = open(output_file, O_CREAT | O_RDWR, 0777);
        status = write(fd_output, output, status);
        printf("Процесс ввода/вывода (запись): удалось записать данные (%d): %s\n", status, output);
        close(fd_output);
    }
    return 0;
}