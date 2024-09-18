#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 24/03/2023
 * Software Engineering Project I - Assignment 2
 * Made by Elvin Jiby in VSCode
 *
 * A simple Gantt Chart program in C!
 * NOTE: Check for circular dependency feature does not work properly.
 */

// Task structure definition
struct task{
    char taskName[80]; // Name of the task
    int startMonth; // Start month
    int endMonth; // End month
    int dependencies; // Number of dependencies this task has
    int dependTask[10]; // Stores the task number it is dependent on
};

// Month enum test data
enum{ 
    JAN = 1, 
    FEB, 
    MAR, 
    APR, 
    MAY, 
    JUN, 
    JUL, 
    AUG, 
    SEP, 
    OCT, 
    NOV, 
    DEC 
};

void remove_newline(char *string); // Removes \n from string, prevents input skips
void getTaskInput(struct task *tasks, int taskAmnt); // Function to get user input for the tasks
void editTaskInput(struct task *tasks, int taskAmnt, char *taskTemp); // Function for the Edit Option
void testTask(struct task *tasks, int taskAmnt, char *taskTemp); // Function to test for critical path
void check_critical_path(struct task *tasks, int taskID, int *visited, int *critical_flag, int *path, int path_index); // Checks critical path
int check_circular(int taskID, int *path, int path_index); //Checks if the task number is in the path (incase of circular depend.)
void printGantt(struct task *tasks, int taskAmnt); // Prints out a Gantt Diagram
void taskOptions(struct task *tasks, int taskAmnt, char *taskTemp); // Function to display Edit/Test/Quit options for the Gantt Diagram
void printXTask(int startMonth, int endMonth); // Prints | XXX | and |    | for relevant task
void printDependency(int *dependencies, int amntDependency); // Prints the dependency numbers for each task
void clear_screen(); // Function to clear the screen

int main(void)
{
    // Variables //
    char choice[4]; // Choice array used to store yes/no input
    char taskTemp[37]; // Temp task name storage
    int taskAmnt; // Amount of tasks

    // User Input //
    printf("Welcome to the Gantt Generator\n");
    printf("Would you like to use the text example or create your own on Gantt from scratch? (yes or no)\n");
    scanf("%s", choice);

    if(strcmp(choice, "yes") == 0 || strcmp(choice, "Yes") == 0 || strcmp(choice, "YES") == 0 || strcmp(choice, "y") == 0)
    {
        int months[] = {JAN, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC};
        struct task tasks[10] = // Creates array of structs for each tasks
        {
            {"Find_Bank", months[0], months[1], 0, {}}, 
            {"Get_mortgage_approval", months[1], months[1], 1, {0}},
            {"Draw_Down", months[3], months[5], 2, {0, 1}},
            {"Build_Foundation", months[2], months[3], 1, {2}},
            {"Build_walls", months[3], months[4], 0, {}},
            {"Roof_and_Ceiling", months[4], months[5], 2, {3, 4}},
            {"Plumbing", months[6], months[6], 1, {5}},
            {"Electrics", months[7], months[8], 1, {6}},
            {"Inspect_Build", months[8], months[9], 1, {4}},
            {"Snagging", months[8], months[9], 2, {7, 8}}
        };

        taskAmnt = 10;

        // Test Diagram //
        printGantt(tasks, taskAmnt);

        // Task Options //
        taskOptions(tasks, taskAmnt, taskTemp);
    }

    else if (strcmp(choice, "no") == 0 || strcmp(choice, "No") == 0 || strcmp(choice, "NO") == 0 || strcmp(choice, "n") == 0)
    {
        // Task Amount //
        printf("How many tasks would you like to add? (1-10)\n");
        scanf("%d", &taskAmnt);

            /* Error Handling */
        while(taskAmnt < 1 || taskAmnt > 10)
        {
            printf("Invalid input.\n");
            printf("How many tasks would you like to add? (1-10)\n");
            scanf("%d", &taskAmnt);
        }

        getchar(); //remove newline

        struct task tasks[taskAmnt]; // Creates array of structs for each tasks

        // Ask User to input tasks //
        getTaskInput(tasks, taskAmnt);

        // Display Gantt Diagram //
        printGantt(tasks, taskAmnt);

        // Task Options //
        taskOptions(tasks, taskAmnt, taskTemp); 
    }

    else
    {
        printf("Invalid option.\n");
    }

    return 0;
}

void remove_newline(char *string)  // Removes the \n from the string and adds a \0
{
    int len = strlen(string);
    if (len > 0 && string[len-1] == '\n') 
    {
        string[len-1] = '\0';
    }
}

void getTaskInput(struct task *tasks, int taskAmnt) // Asks user to input task information
{
    // Ask the user to input the tasks and the relevant details
    for (int i = 0; i < taskAmnt; i++) 
    {
        // Task Name //
        printf("\nPlease enter the task name: (max 35 characters)\n");
        fgets(tasks[i].taskName, 80, stdin);
        remove_newline(tasks[i].taskName);

            /* Error Handling */
        while (strlen(tasks[i].taskName) > 35)
        {
            printf("Name is too long. Please try again:\n");
            fgets(tasks[i].taskName, 80, stdin);
            remove_newline(tasks[i].taskName);
        }

        printf("\n");

        // Start Month //
        printf("Start month (1-12):\n");
        scanf("%d", &tasks[i].startMonth);

            /* Error handling */
        while(tasks[i].startMonth < 1 || tasks[i].startMonth > 12)
        {
            printf("Invalid input.\n");
            printf("Start month (1-12):\n");
            scanf("%d", &tasks[i].startMonth);
        }

        // End Month //
        printf("End month (1-12):\n");
        scanf("%d", &tasks[i].endMonth);

            /* Error handling */
        while(tasks[i].endMonth < 1 || tasks[i].endMonth > 12)
        {
            printf("Invalid input.\n");
            printf("End month (1-12):\n");
            scanf("%d", &tasks[i].endMonth);
        }

        while(tasks[i].endMonth < tasks[i].startMonth)
        {
            printf("End month cannot be before the start month.\n");
            printf("End month (1-12):\n");
            scanf("%d", &tasks[i].endMonth);
        }

        // Dependencies //
        printf("Enter how many dependencies this task has:\n");
        scanf("%d", &tasks[i].dependencies);

            /* Error Handling */
        while (tasks[i].dependencies < 0)
        {
            printf("Invalid input.\n");
            printf("Enter how many dependencies this task has:\n");
            scanf("%d", &tasks[i].dependencies);
        }

        getchar();

        if (tasks[i].dependencies > 0 && tasks[i].dependencies <= 10)
        {
            for (int j = 0; j<tasks[i].dependencies; j++)
            {
                printf("Enter dependent task:\n");
                scanf("%d", &tasks[i].dependTask[j]);
                getchar();
            }
        }
    }
}

void editTaskInput(struct task *tasks, int taskAmnt, char *taskTemp) // Edits task
{
    printf("\nPlease enter the task name you wish to change exactly:\n");
    fgets(taskTemp, 80, stdin);
    remove_newline(taskTemp);

    int flag = -1;
    int taskIndex = 0;

    for (int i = 0; i<taskAmnt; i++)
    {
        if(strcmp(taskTemp, tasks[i].taskName) == 0)
        {
            flag = 1; //Task name found
            taskIndex = i;
            i = taskAmnt;
        }
        else
        {
            flag = -1; //Task name not found
        }
    }

    // Task not found //
    while (flag == -1)
    {
        printf("Task not found.\n");
        editTaskInput(tasks, taskAmnt, taskTemp);
    }
    
    if (flag == 1)
    {
        printf("Please enter the new task name or write its old one:\n");
        fgets(tasks[taskIndex].taskName, 80, stdin);
        remove_newline(tasks[taskIndex].taskName);

        while (strlen(tasks[taskIndex].taskName) > 35)
        {
            printf("Name is too long. Please try again:\n");
            fgets(tasks[taskIndex].taskName, 80, stdin);
            remove_newline(tasks[taskIndex].taskName);
        }

        printf("\n");

        // Start Month //
        printf("Start month (1-12):\n");
        scanf("%d", &tasks[taskIndex].startMonth);

            /* Error handling */
        while(tasks[taskIndex].startMonth < 1 || tasks[taskIndex].startMonth > 12)
        {
            printf("Invalid input.\n");
            printf("Start month (1-12):\n");
            scanf("%d", &tasks[taskIndex].startMonth);
        }

        // End Month //
        printf("End month (1-12):\n");
        scanf("%d", &tasks[taskIndex].endMonth);

            /* Error handling */
        while(tasks[taskIndex].endMonth < 1 || tasks[taskIndex].endMonth > 12)
        {
            printf("Invalid input.\n");
            printf("End month (1-12):\n");
            scanf("%d", &tasks[taskIndex].endMonth);
        }

        while(tasks[taskIndex].endMonth < tasks[taskIndex].startMonth)
        {
            printf("End month cannot be before the start month.\n");
            printf("End month (1-12):\n");
            scanf("%d", &tasks[taskIndex].endMonth);
        }

        // Dependencies //
        printf("Enter how many dependencies this task has:\n");
        scanf("%d", &tasks[taskIndex].dependencies);

            /* Error Handling */
        while (tasks[taskIndex].dependencies < 0)
        {
            printf("Invalid input.\n");
            printf("Enter how many dependencies this task has:\n");
            scanf("%d", &tasks[taskIndex].dependencies);
        }

        getchar();

        if (tasks[taskIndex].dependencies > 0 && tasks[taskIndex].dependencies <= 10)
        {
            for (int j = 0; j<tasks[taskIndex].dependencies; j++)
            {
                printf("Enter dependent task:\n");
                scanf("%d", &tasks[taskIndex].dependTask[j]);
                getchar();
            }
        }

        clear_screen(); // Clears screen

        // Display Gantt Diagram //
        printGantt(tasks, taskAmnt);
        // Task Options //
        taskOptions(tasks, taskAmnt, taskTemp);
    }
}

void printGantt(struct task *tasks, int taskAmnt) // Prints Gantt diagram with task information given
{
    printf("____________________________________________________________________________________________________________________________________________________________\n");                           
    printf("%148s\n", "|  Jan  |  Feb  |  Mar  |  Apr  |  May  |  Jun  |  Jul  |  Aug  |  Sep  |  Oct  |  Nov  |  Dec  |   Dependencies");
    printf("------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    // Display Tasks //
    for (int i = 0; i<taskAmnt; i++)
    {
        printf("%s%*s", tasks[i].taskName, 36 - strlen(tasks[i].taskName), "");       
        printXTask(tasks[i].startMonth, tasks[i].endMonth);
        printDependency(tasks[i].dependTask, tasks[i].dependencies);  
        printf("------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    }

    printf("\nIf you wish to edit the Gantt please type 'edit'\n");
    printf("If you wish to run a test, type 'test'\n");
    printf("Or to exit, type 'quit' and then press enter to execute your option.\n");
}

void taskOptions(struct task *tasks, int taskAmnt, char *taskTemp) // Displays Edit/Test/Quit options for user to select
{
    char choice2[5]; // Choice array 
    scanf("%s", choice2);
    getchar();

    if(strcmp(choice2, "edit") == 0)
    {
        editTaskInput(tasks, taskAmnt, taskTemp); //If edit is chosen, do editTask procedure
    }
    else if (strcmp(choice2, "test") == 0)
    {
        testTask(tasks, taskAmnt, taskTemp);
    }
    else if (strcmp(choice2, "quit") == 0)
    {
        printf("\n");
        printf(" ____________________ \n");
        printf("/  |  |  |  |  |  |  \\ \n");
        printf("\\--------------------/ \n");
        printf(" \\  \\  \\  |  / /  / /  \n");
        printf("  \\  \\  \\ | / /  / /  \n");
        printf("   \\  \\  \\|/ /  / /   \n");
        printf("    \\  \\  | /  / /    \n");
        printf("     \\  \\ |/  / /     \n");
        printf("      \\  \\|  / /      \n");
        printf("       \\  | / /       \n");
        printf("        \\ |/ /        \n");
        printf("         \\| /         \n");
        printf("          \\/          \n");
        printf(" Chaos Emerald - Ascii Art by Elvin Jiby \n");
        exit(0); // Terminate the program
    }
    else
    {
        printf("Invalid input.\n");
        taskOptions(tasks, taskAmnt, taskTemp); 
    }    
}

void testTask(struct task *tasks, int taskAmnt, char *taskTemp) // Tests a particular task for a critical path
{
    printf("\nPlease enter the task name to test:\n");
    fgets(taskTemp, 80, stdin);
    remove_newline(taskTemp);

    int flag = -1;
    int taskIndex = 0;

    for (int i = 0; i<taskAmnt; i++)
    {
        if(strcmp(taskTemp, tasks[i].taskName) == 0)
        {
            flag = 1; //Task name found
            taskIndex = i;
            i = taskAmnt;
        }
        else
        {
            flag = -1; //Task name not found
        }
    }

    // Task not found //
    while (flag == -1)
    {
        printf("Task not found.\n");
        testTask(tasks, taskAmnt, taskTemp);
    }

    if (flag == 1)
    {
        int visitedTasks[taskAmnt];
        // Initialise all value of visited to 0 (no task has been visited)
        for (int i = 0; i<taskAmnt; i++)
        {
            visitedTasks[i] = 0;
        }

        int path[taskAmnt];
        // Initialise all values of path to -1 (task hasn't been added to current path)
        for (int i = 0; i<taskAmnt; i++)
        {
            path[i] = -1;
        }

        int critical_flag = 0; //Variable that indicates if a critical path has been found or not

        for(int i = 0; i < taskAmnt; i++) 
        {
            if(visitedTasks[i] == 0) 
            {
                check_critical_path(tasks, taskIndex, visitedTasks, &critical_flag, path, 0);
            }
        }

        if(critical_flag == 0) 
        {
            printf("No circular dependencies detected.\n");
        }
    	
        printf("\nIf you wish to edit the Gantt please type 'edit'\n");
        printf("If you wish to run a test, type 'test'\n");
        printf("Or to exit, type 'quit' and then press enter to execute your option.\n");
        taskOptions(tasks, taskAmnt, taskTemp);
    }
}

int check_circular(int taskID, int *path, int path_index) // Checks if the visited task is already on a path
{
    for(int i = 0; i < path_index; i++) 
    {
        if(path[i] == taskID) 
        {
            return 1;
        }
    }
    return 0;
}

void check_critical_path(struct task *tasks, int taskID, int *visitedTasks, int *critical_flag, int *path, int path_index) // Tries to find a critical path for task
{
    // Mark the current task as visited
    visitedTasks[taskID] = 1;
    // Add the current task to the path
    path[path_index] = taskID;

    printf("%d -> ", taskID+1);
    visitedTasks[taskID] = 1;

    // Loop through the dependencies of the current task
    for(int i = 0; i < tasks[taskID].dependencies; i++) 
    {
        int dependent_TaskID = tasks[taskID].dependTask[i];

        // If the dependency hasn't been visited, go deeper
        if(visitedTasks[dependent_TaskID] == 0) 
        {
            check_critical_path(tasks, dependent_TaskID, visitedTasks, critical_flag, path, path_index + 1);
        }

        // Otherwise if it has already been visited and is in the path, there is a circular dependency
        else 
        {
            printf("( !!!!! Warning: Potential Circular Dependency Found !!!!! ) \n");
            if ((check_circular(taskID, path, path_index)) == 1)
            {
                printf("!!! Circular dependency found !!!\n");
                *critical_flag = 1;
                return;
            }
        }
    }

    // Remove the current task from the path
    path[path_index] = -1;
    // Mark the current task as unvisited
    visitedTasks[taskID] = 0;

    // If the current task is at the end and there's no circular dependency, it has a critical path
    if(*critical_flag == 0) 
    {
        for(int i = 0; i <= path_index; i++) 
        {
            printf("%d", path[i]);
            if(i < path_index) 
            {
                printf(" -> ");
            }
        }
        printf("\n");
    }

    return;

    /*Note: Unable to get this function to work.*/
}

void printXTask(int startMonth, int endMonth) // Prints either a | XXX | or |   | for the relevant task
{
    for(int i = 1; i<=12; i++) // Iterate through each month 
    {
        if (i == startMonth) //If the start month is reached, begin a procedure
        {
            for(int j = startMonth; j<=12; j++) // Iterate from that month onwards
            { 
                if (j == endMonth) //If the end month is reached, print the last | XXX |
                {
                    printf("|  XXX  |");
                    j = 13;
                    i = 13;

                    int k = endMonth;
                    while (k < 12) // If there are still more months afterwards, just print blank spaces
                    {
                        printf("       |");
                        k++;
                    }

                }

                else
                {
                    printf("|  XXX  "); // If the end month hasn't been reached, simply keep printing | XXX | for that month
                }
            }
        }

        else // Otherwise just print blank spaces for that month
        {
            printf("|       ");
        }
    }
}

void printDependency(int *dependencies, int amntDependency) // Prints the dependency numbers
{
    if(amntDependency != 0)
    {
        for (int i = 0; i<amntDependency; i++)
        {
            printf(" %d", dependencies[i]); // Prints the dependency numbers
        }

        printf("\n");
    }

    else
    {
        printf("\n"); // If there are no dependencies for that task, print a new line and move to next task (if any)
    }
}

void clear_screen() // Clears the screen depending on OS
{
  #ifdef _WIN32
    system("cls");
  #else
    system("clear");
  #endif
}