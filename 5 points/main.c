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
    pid_t child_id_input, child_id_task, child_id_output;
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
            child_id_output = fork();
            if (child_id_output < 0)
            {
                printf("Не удалось создать процесс вывода.\n");
                exit(-1);
            }
            if (child_id_output != 0 && child_id_task != 0 && child_id_input != 0)
            {
                printf("Cоздан процесс вывода!\n");
            }
            else
            {
                printf("Процесс записи: Приступаем к чтению из именованного канала!\n");
                task_output_fd = open(task_to_output, O_RDONLY);
                status = read(task_output_fd, output, buf);
                printf("Процесс записи: удалось получить данные (%d): %s\n", status, output);
                if (close(task_output_fd) < 0)
                {
                    printf("Процесс записи: Не могу закрыть неименованный канал для чтения!\n");
                    exit(-1);
                }
                fd_output = open(output_file, O_CREAT | O_RDWR, 0777);
                status = write(fd_output, output, status);
                printf("Процесс записи: удалось записать данные (%d): %s\n", status, output);
                close(fd_output);
                unlink(task_to_output);
            }
        }
        else
        {
            printf("Процесс решения: Приступаем к чтению из именованного канала!\n");
            input_task_fd = open(input_to_task, O_RDONLY);
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
            status = write(task_output_fd, output, size_of_output);
            printf("Процесс решения: удалось записать данные (%d): %s\n", status, output);
            if (close(task_output_fd) < 0)
            {
                printf("Процесс решения: Не могу закрыть именованный канал для записи!\n");
                exit(-1);
            }
            printf("Процесс решения: Завершил запись!\n");
            unlink(task_to_output);
            unlink(input_to_task);
        }
    }
    else
    {
        printf("Процесс ввода: Процесс ввода начинает считывание из файла %s!\n", input_file);
        fd_input = open(input_file, O_RDONLY);
        status = read(fd_input, input, buf);
        if (status < 0)
        {
            printf("Процесс ввода: Не удалось считать входные данные из %s!\n", input_file);
            exit(-1);
        }
        printf("Процесс ввода: удалось получить данные (%d): %s\n", status, input);
        printf("Процесс ввода: Приступаем к записи в именованный канал!\n");
        input_task_fd = open(input_to_task, O_WRONLY);
        status = write(input_task_fd, input, buf);
        printf("Процесс ввода: удалось записать данные (%d): %s\n", status, input);
        if (close(input_task_fd) < 0)
        {
            printf("Процесс ввода: Не могу закрыть именованный канал!\n");
            exit(-1);
        }
        printf("Процесс ввода: Завершил запись!\n");
        close(fd_input);
        unlink(input_to_task);
    }
    return 0;
}