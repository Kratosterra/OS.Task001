#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include "../functions/function.c"

int buf = 5000;

int main(int argc, char *argv[])
{
    char input[5000];
    char output[5000];
    int status;
    pid_t child_id_input, child_id_task, child_id_output;
    int input_to_task[2];
    int task_to_output[2];
    int fd_input;
    int fd_output;
    if (argc != 3) // != 3
    {
        printf("Ошибка, задайте в аргументах 2 имени файла");
        exit(-1);
    }
    char *input_file = argv[1];
    char *output_file = argv[2];
    if (pipe(input_to_task) < 0)
    {
        printf("Не удаось создать неименованный канал между вводом и решением задачи!\n");
        exit(-1);
    }
    if (pipe(task_to_output) < 0)
    {
        printf("Не удалось создать неименованный канал между решением задачи и выводом!\n");
        exit(-1);
    }

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
                printf("Процесс записи: Приступаем к чтению из неименованного канала!\n");
                if (close(task_to_output[1]) < 0)
                {
                    printf("Процесс записи: Не могу закрыть неименованный канал для записи!\n");
                    exit(-1);
                }
                status = read(task_to_output[0], output, buf);
                printf("Процесс записи: удалось получить данные (%d): %s\n", status, output);
                if (close(task_to_output[0]) < 0)
                {
                    printf("Процесс записи: Не могу закрыть неименованный канал для чтения!\n");
                    exit(-1);
                }
                fd_output = open(output_file, O_CREAT | O_RDWR, 0777);
                status = write(fd_output, output, status);
                printf("Процесс записи: удалось записать данные (%d): %s\n", status, output);
                close(fd_output);
                close(task_to_output);
            }
        }
        else
        {
            printf("Процесс решения: Приступаем к чтению из неименованного канала!\n");
            if (close(input_to_task[1]) < 0)
            {
                printf("Процесс решения: Не могу закрыть неименованный канал для записи!\n");
                exit(-1);
            }
            status = read(input_to_task[0], input, buf);
            printf("Процесс решения: удалось получить данные (%d): %s\n", status, input);
            if (close(input_to_task[0]) < 0)
            {
                printf("Процесс решения: Не могу закрыть неименованный канал для чтения!\n");
                exit(-1);
            }
            int size_of_output = capitalizeWords(input, output);
            printf("Процесс решения: Завершил решение. Ответ: %s!\n", output);
            printf("Процесс решения: Приступаю к записи в новый неименованный канал для записи ответа\n");
            if (close(task_to_output[0]) < 0)
            {
                printf("Процесс ввода: Не могу закрыть неименованный канал для чтения!\n");
                exit(-1);
            }
            status = write(task_to_output[1], output, size_of_output);
            printf("Процесс решения: удалось записать данные (%d): %s\n", status, output);
            if (close(task_to_output[1]) < 0)
            {
                printf("Процесс решения: Не могу закрыть неименованный канал для записи!\n");
                exit(-1);
            }
            printf("Процесс решения: Завершил запись!\n");
            close(input_to_task);
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
        printf("Процесс ввода: Приступаем к записи в неименованный канал!\n");
        if (close(input_to_task[0]) < 0)
        {
            printf("Процесс ввода: Не могу закрыть неименованный канал для чтения!\n");
            exit(-1);
        }
        status = write(input_to_task[1], input, buf);
        printf("Процесс ввода: удалось записать данные (%d): %s\n", status, input);
        if (close(input_to_task[1]) < 0)
        {
            printf("Процесс ввода: Не могу закрыть неименованный канал для записи!\n");
            exit(-1);
        }
        printf("Процесс ввода: Завершил запись!\n");
        close(fd_input);
    }
    return 0;
}