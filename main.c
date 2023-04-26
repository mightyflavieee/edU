#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct file_list {
    char *line;
    struct file_list *next;
};

struct stack {
    struct file_list *init_pointer;
    struct file_list *tail_pointer;
    struct stack *prev;
};

struct command_list {
    int ind1;
    int ind2;
    int num_substituted_elem;
    char command_type;
    struct file_list *prev_node;
    struct file_list *succ_node;
    struct file_list *substitute;
    struct file_list *substitute_tail;
    struct command_list *next;
    struct command_list *prev;
};

int elements_in_list = 0;
int end_program = 0;
int commands_number = 0;
int commands_undo = 0;
int commands_redo = 0;
int first_command_used = 0;
struct stack *stack_change = NULL;
struct stack *stack_delete = NULL;
struct stack *stack_undo_redo = NULL;

void add_in_stack(struct file_list *block_head, struct file_list *block_tail, char command_type) {
    struct stack *new_node = (struct stack *) malloc(sizeof(struct stack));
    struct stack *last_node = NULL;

    switch (command_type) {
        case 'c':
            last_node = stack_change;
            new_node->init_pointer = block_head;
            new_node->tail_pointer = block_tail;
            if (stack_change == NULL) {
                stack_change = new_node;
                stack_change->prev = NULL;
            } else {
                stack_change = new_node;
                stack_change->prev = last_node;
            }
            break;
        case 'd':
            last_node = stack_delete;
            new_node->init_pointer = block_head;
            new_node->tail_pointer = block_tail;
            if (stack_delete == NULL) {
                stack_delete = new_node;
                stack_delete->prev = NULL;
            } else {
                stack_delete = new_node;
                stack_delete->prev = last_node;
            }
            break;
        case 'u':
            last_node = stack_undo_redo;
            new_node->init_pointer = block_head;
            new_node->tail_pointer = block_tail;
            if (stack_undo_redo == NULL) {
                stack_undo_redo = new_node;
                stack_undo_redo->prev = NULL;
            } else {
                stack_undo_redo = new_node;
                stack_undo_redo->prev = last_node;
            }
            break;
        default:
            break;
    }
}

void insert_line(struct file_list **file) {
    char *line_acquisition = malloc(1025 * sizeof(char));
    char *line;
    struct file_list *node = *file;

    fgets(line_acquisition, 1025, stdin);
    line = realloc(line_acquisition, strlen(line_acquisition) + 1);
    node->line = line;
}

void add_list_tail(struct file_list **file, struct file_list **tail) {
    struct file_list *file_new_node = (struct file_list *) malloc(sizeof(struct file_list));
    struct file_list *file_last_node;

    insert_line(&file_new_node);
    file_new_node->next = NULL;

    if (*file == NULL) {
        *file = file_new_node;
        // aggiorno coda
        *tail = file_new_node;
    } else {
        // aggiungo un nodo in coda
        file_last_node = *tail;
        file_last_node->next = file_new_node;
        // aggiorno coda
        *tail = file_new_node;
    }
}

void change(int ind1, int ind2, struct file_list **file, struct file_list **tail, struct command_list **commands_list) {
    struct command_list *commandList = (struct command_list *) malloc(sizeof(struct command_list));
    struct command_list *temp = *commands_list;
    struct file_list *block = NULL;
    struct file_list *block_head = NULL;
    struct file_list *block_tail = NULL;
    struct file_list *file_last_node = NULL;
    struct file_list *prev_node = NULL;
    int j = 0, i = 0, flag = 0, num_elements = ind2 - ind1 + 1;

    getchar();

    if (commands_number == 0) {
        commandList->prev = NULL;
    } else {
        commandList->prev = *commands_list;
    }
    commandList->ind1 = ind1;
    commandList->ind2 = ind2;
    commandList->command_type = 'c';
    commandList->next = NULL;

    for (i = 0; i < (ind2 - ind1 + 1); ++i) {
        if (block == NULL) { flag = 1; }
        add_list_tail(&block, &block_tail);
        if (flag == 1) { block_head = block; }
    }
    block = block_head;
    add_in_stack(block_head, block_tail, 'c');
    if (*file == NULL) {
        *file = block;
        *tail = block_tail;
        commandList->num_substituted_elem = 0;
        commandList->substitute = NULL;
        commandList->prev_node = NULL;
        commandList->succ_node = NULL;
        elements_in_list = num_elements;
    } else {
        if (ind1 > elements_in_list) {
            file_last_node = *tail;
            file_last_node->next = block;
            commandList->num_substituted_elem = 0;
            commandList->substitute = NULL;
            commandList->prev_node = *tail;
            commandList->succ_node = NULL;
            *tail = block_tail;
            elements_in_list = elements_in_list + num_elements;
        } else {
            file_last_node = *file;
            for (j = 1; j < ind1-1; j++) { file_last_node = file_last_node->next; }
            if (ind1 == 1) {
                commandList->substitute = file_last_node;
                commandList->prev_node = NULL;
            } else {
                commandList->substitute = file_last_node->next;
                commandList->prev_node = file_last_node;
            }
            if (ind2 >= elements_in_list) {
                commandList->succ_node = NULL;
                commandList->substitute_tail = *tail;
                if (ind1 == 1) {
                    file_last_node = block;
                    *file = file_last_node;
                    commandList->num_substituted_elem = elements_in_list;
                } else {
                    file_last_node->next = block;
                    commandList->num_substituted_elem = elements_in_list - ind1 + 1;
                }
                *tail = block_tail;
                elements_in_list = elements_in_list + (ind2 - elements_in_list);
            } else {
                prev_node = file_last_node;
                commandList->num_substituted_elem = ind2 - ind1 + 1;
                while (j != ind2) {
                    file_last_node = file_last_node->next;
                    j++;
                }
                commandList->substitute_tail = file_last_node;
                if (ind1 == 1) {
                    prev_node = block;
                    block_tail->next = file_last_node->next;
                    *file = prev_node;
                } else {
                    prev_node->next = block;
                    block_tail->next = file_last_node;
                }
                commandList->succ_node = file_last_node->next;
            }
        }
    }

    if (commands_number == 0) {
        *commands_list = commandList;
    } else {
        temp->next = commandList;
    }
    commands_number++;
    commands_undo++;
    *commands_list = commandList;

    // catturo il punto che indica la fine del comando
    getchar();
}

void delete(int ind1, int ind2, struct file_list **file, struct file_list **tail, struct command_list **commands_list) {
    struct file_list *cur, *prev, *succ;
    struct command_list *commandList = (struct command_list *) malloc(sizeof(struct command_list));
    struct command_list *temp = *commands_list;

    int flag = 0;
    struct file_list *block_head = NULL;
    struct file_list *block_tail = NULL;

    if (commands_number == 0) {
        commandList->prev = NULL;
    } else {
        commandList->prev = *commands_list;
    }
    commandList->ind1 = ind1;
    commandList->ind2 = ind2;
    commandList->command_type = 'd';
    commandList->substitute = NULL;
    commandList->substitute_tail = NULL;
    commandList->next = NULL;

    prev = NULL;
    cur = *file;

    if (ind1 != 0 && ind2 != 0 && ind1 <= elements_in_list) {
        if (cur != NULL) {
            succ = cur->next;
            for (int i = 1; i < ind1; ++i) {prev = cur; cur = succ; succ = succ->next; }
            commandList->prev_node = prev;
            if (ind2 >= elements_in_list) {
                if (prev != NULL) {
                    *tail = prev;
                    flag = 1;
                } else {
                    *tail = NULL;
                }
                ind2 = elements_in_list;
            }
        }
        elements_in_list = elements_in_list - (ind2 - ind1 + 1);
        commandList->num_substituted_elem = ind2 - ind1 + 1;

        for (int i = 0; i < (ind2 - ind1 + 1); ++i) {
            if (i == 0) {
                block_head = cur;
            } else {
                cur = succ;
                if (succ != NULL) {
                    succ = succ->next;
                }
            }
        }
        if (cur != NULL) {
            cur->next = NULL; // PER GARANTIRE CHE IL BLOCCO NON SIA PIU LEGATO A NULLA (PER NON AVERE ERRORI IN UNDO/REDO)
        }
        block_tail = cur;
        add_in_stack(block_head, block_tail, 'd');
        commandList->succ_node = succ;

        if (flag != 1) {
            if (prev == NULL) {
                *file = succ;
            } else {
                prev->next = succ;
            }
        }
    } else {
        block_head = NULL;
        block_tail = NULL;
        add_in_stack(block_head, block_tail, 'd');
        commandList->num_substituted_elem = 0;
        commandList->succ_node = NULL;
        commandList->prev_node = NULL;
    }

    if (commands_number == 0) {
        *commands_list = commandList;
    } else {
        temp->next = commandList;
    }
    commands_number++;
    commands_undo++;
    *commands_list = commandList;
}

void print(int ind1, int ind2, struct file_list *file, struct file_list *tail) {
    struct file_list *file_first_node;
    file_first_node = file;
    if (ind1 == 0 && ind2 == 0) {
        printf(".\n");
    } else {
        if (ind1 != elements_in_list) {
            for (int j = 1; j < ind1; ++j) { file_first_node = file_first_node->next; }
        } else {
            file_first_node = tail;
        }
        for (int i = 0; i < (ind2 - ind1 + 1); ++i) {
            if (file_first_node == NULL) {
                printf(".\n");
            } else {
                printf("%s", file_first_node->line);
                file_first_node = file_first_node->next;
            }
        }
    }
}

void undo(struct file_list **file, struct file_list **tail, struct command_list **commands, struct command_list **command_position, int num_undo) {
    struct command_list *scroll_commands;
    struct file_list *file_temp = *file;

    if (*command_position == NULL) {
        scroll_commands = *commands;
    } else {
        scroll_commands = *command_position;
    }

    if (scroll_commands != NULL) {
        for (int i = 0; i < num_undo; ++i) {
            if (scroll_commands != NULL) {
                if (scroll_commands->command_type == 'c') {

                    // sposto da stack_change a stack_undo_redo
                    add_in_stack(stack_change->init_pointer, stack_change->tail_pointer, 'u');
                    stack_change = stack_change->prev;

                    // se ho solo aggiunto in coda
                    if (scroll_commands->substitute == NULL) {
                        if (scroll_commands->prev_node != NULL) {
                            scroll_commands->prev_node->next = NULL;
                            *tail = scroll_commands->prev_node;
                            elements_in_list = elements_in_list - (scroll_commands->ind2 - scroll_commands->ind1 + 1);
                        } else {
                            *file = NULL;
                            *tail = NULL;
                            elements_in_list = 0;
                        }
                    } else {
                        // se ho sostituito qualche riga (cambio il blocco)
                        if (scroll_commands->prev_node != NULL) {
                            scroll_commands->prev_node->next = scroll_commands->substitute;
                        } else {
                            // se scroll_commands->prev_node = NULL devo cambiare la testa
                            *file = scroll_commands->substitute;
                        }
                        scroll_commands->substitute_tail->next = scroll_commands->succ_node;
                        if (scroll_commands->substitute_tail->next == NULL) {
                            *tail = scroll_commands->substitute_tail;
                        }
                        elements_in_list = elements_in_list - (scroll_commands->ind2 - scroll_commands->ind1 + 1) + scroll_commands->num_substituted_elem;
                    }

                } else {
                    add_in_stack(stack_delete->init_pointer, stack_delete->tail_pointer, 'u');
                    stack_delete = stack_delete->prev;

                    if (stack_undo_redo->init_pointer != NULL && stack_undo_redo->tail_pointer != NULL) {
                        if (scroll_commands->prev_node != NULL) {
                            scroll_commands->prev_node->next = stack_undo_redo->init_pointer;
                            stack_undo_redo->tail_pointer->next = scroll_commands->succ_node;
                            elements_in_list =
                                    elements_in_list + (scroll_commands->ind2 - scroll_commands->ind1 + 1);
                        } else {
                            *file = stack_undo_redo->init_pointer;
                            if (scroll_commands->succ_node != NULL) {
                                stack_undo_redo->tail_pointer->next = scroll_commands->succ_node;
                            } else {
                                stack_undo_redo->tail_pointer->next = NULL;
                                *tail = stack_undo_redo->tail_pointer;
                            }
                            elements_in_list = elements_in_list + scroll_commands->num_substituted_elem;
                        }
                    }
                }
            }
            if (scroll_commands->prev != NULL) {
                scroll_commands = scroll_commands->prev;
            } else {
                first_command_used = 1;
            }
            commands_undo--;
            commands_redo++;
        }
    }
    *command_position = scroll_commands;
}

void redo(struct file_list **file, struct file_list **tail, struct command_list **commands, struct command_list **command_position, int num_redo) {
    struct command_list *scroll_commands = *command_position;
    struct command_list *prev = NULL;
    struct file_list *file_temp = *file;

    if (scroll_commands != NULL) {

        if (scroll_commands->next != NULL) {
            if (scroll_commands->prev != NULL) {
                scroll_commands = scroll_commands->next;
                first_command_used = 0;
            } else {
                if (first_command_used == 1) {
                    first_command_used = 0;
                } else {
                    scroll_commands = scroll_commands->next;
                }
            }
            for (int i = 0; i < num_redo; ++i) {
                if (scroll_commands != NULL) {
                    if (scroll_commands->command_type == 'c') {

                        add_in_stack(stack_undo_redo->init_pointer, stack_undo_redo->tail_pointer, 'c');

                        if (scroll_commands->prev_node == NULL) {
                            if (*file == NULL) {
                                *file = stack_undo_redo->init_pointer;
                                stack_undo_redo->tail_pointer->next = NULL;
                                *tail = stack_undo_redo->tail_pointer;
                                elements_in_list = scroll_commands->ind2 - scroll_commands->ind1 + 1;
                            } else {
                                *file = stack_undo_redo->init_pointer;
                                stack_undo_redo->tail_pointer->next = scroll_commands->succ_node;
                                if (stack_undo_redo->tail_pointer->next == NULL) {
                                    *tail = stack_undo_redo->tail_pointer;
                                }
                                elements_in_list = elements_in_list + (scroll_commands->ind2 - scroll_commands->ind1 + 1) - scroll_commands->num_substituted_elem;
                            }
                        } else {
                            scroll_commands->prev_node->next = stack_undo_redo->init_pointer;
                            stack_undo_redo->tail_pointer->next = scroll_commands->succ_node;
                            if (stack_undo_redo->tail_pointer->next == NULL) {
                                *tail = stack_undo_redo->tail_pointer;
                            }
                            elements_in_list = elements_in_list + (scroll_commands->ind2 - scroll_commands->ind1 + 1) - scroll_commands->num_substituted_elem;
                        }

                    } else {

                        add_in_stack(stack_undo_redo->init_pointer, stack_undo_redo->tail_pointer, 'd');

                        if (scroll_commands->prev_node == NULL) {
                            if (scroll_commands->succ_node == NULL) {
                                *file = NULL;
                                *tail = NULL;
                                elements_in_list = 0;
                            } else {
                                *file = scroll_commands->succ_node;
                                elements_in_list =
                                        elements_in_list - (scroll_commands->ind2 - scroll_commands->ind1 + 1);
                            }
                        } else {
                            if (scroll_commands->succ_node == NULL) {
                                scroll_commands->prev_node->next = NULL;
                                *tail = scroll_commands->prev_node;
                            } else {
                                scroll_commands->prev_node->next = scroll_commands->succ_node;
                            }
                            elements_in_list = elements_in_list - (scroll_commands->ind2 - scroll_commands->ind1 + 1);
                        }

                    }
                    commands_redo--;
                    commands_undo++;
                    prev = scroll_commands;
                    scroll_commands = scroll_commands->next;
                    stack_undo_redo = stack_undo_redo->prev;
                }
            }
            *command_position = prev;
        }

    }
}

void delete_commands(struct command_list **commands, struct command_list **command_position) {
    struct command_list *prev = *command_position;
    struct command_list *to_delete = prev->next;
    struct command_list *succ = to_delete->next;
    struct command_list *new_head;
    struct command_list *temp = prev;
    int flag = 0;

    if (prev->prev == NULL && first_command_used == 1) {
        flag = 1;
    }

    while (1) {
        if (flag == 1) {
            new_head = prev->next;
            free(prev);
            prev = new_head;
            if (prev == NULL) {
                *command_position = NULL;
                *commands = NULL;
                commands_number = 0;
                break;
            }
        } else {
            prev->next = to_delete->next;
            if (to_delete->next != NULL) {
                to_delete->next->prev = prev;
            }
            free(to_delete);
            to_delete = succ;
            if (succ != NULL) {
                succ = succ->next;
            } else {
                *command_position = prev;
                *commands = prev;
                commands_number--;
                break;
            }
        }
        commands_number--;
    }
    commands_redo = 0;
    first_command_used = 0;

    // mancano i casi in cui ho un solo comando (NULL<-command->NULL) e il caso in cui sono tornato fino all'inizio (NULL<-command->next_command)
}

void new_minimize_undo_redo(int num, struct file_list **file, struct file_list **tail, struct command_list **commands) {
    int i = 0, flag = 0, ind1 = 0, ind2 = 0;
    int num_undo = 0, num_redo = 0, pila_undo = 0, pila_redo = 0;
    char command;
    struct command_list *commands_position = NULL;

    pila_undo = commands_undo;
    pila_redo = commands_redo;
    if (num > 0) {
        if (num > commands_undo) {
            num_undo = commands_undo;
        } else {
            num_undo = num;
        }
        pila_undo = pila_undo - num_undo;
        pila_redo = pila_redo + num_undo;
    } else {
        if (-num > commands_redo) {
            num_redo = commands_redo;
        } else {
            num_redo = -num;
        }
        pila_redo = pila_redo - num_redo;
        pila_undo = pila_undo + num_redo;
    }

    getchar();

    while (1) {
        command = getchar();
        if (command != 'q') {
            if (command == ',' || command == 'r' || command == 'u') {
                if (command == ',') {
                    flag = 1;
                    i = 0;
                } else {
                    if (command == 'r') {
                        if (ind1 > pila_redo) {
                            num_redo = num_redo + pila_redo;
                            pila_undo = pila_undo + pila_redo;
                            pila_redo = 0;
                        } else {
                            num_redo = num_redo + ind1;
                            pila_redo = pila_redo - ind1;
                            pila_undo = pila_undo + ind1;
                        }

                        getchar();
                        i = 0; flag = 0; ind1 = 0;
                    } else {
                        if (ind1 > pila_undo) {
                            num_undo = num_undo + pila_undo;
                            pila_redo = pila_redo + pila_undo;
                            pila_undo = 0;
                        } else {
                            num_undo = num_undo + ind1;
                            pila_undo = pila_undo - ind1;
                            pila_redo = pila_redo + ind1;
                        }

                        getchar();
                        i = 0; flag = 0; ind1 = 0;
                    }
                }
            } else {
                if (command == 'c' || command == 'd' || command == 'p') {

                    num = num_undo - num_redo;

                    if (command == 'c') {

                        if (num != 0) {
                            if (num > 0) {
                                undo(file, tail, commands, &commands_position, num);
                            } else {
                                redo(file, tail, commands, &commands_position, -num);
                            }
                        }

                        if (commands_position != NULL) {
                            if (commands_position->next != NULL) {
                                delete_commands(commands, &commands_position);
                            }
                            stack_undo_redo = NULL;

                            if (*commands == NULL) {
                                stack_change = NULL;
                                stack_delete = NULL;
                            }

                        }

                        change(ind1, ind2, file, tail, commands);
                        getchar();
                        break;
                    } else {
                        if (command == 'd') {

                            if (num != 0) {
                                if (num > 0) {
                                    undo(file, tail, commands, &commands_position, num);
                                } else {
                                    redo(file, tail, commands, &commands_position, -num);
                                }
                            }

                            if (commands_position != NULL) {
                                if (commands_position->next != NULL) {
                                    delete_commands(commands, &commands_position);
                                }
                                stack_undo_redo = NULL;

                                if (*commands == NULL) {
                                    stack_change = NULL;
                                    stack_delete = NULL;
                                }

                            }

                            delete(ind1, ind2, file, tail, commands);
                            getchar();
                            break;
                        } else {

                            if (num != 0) {
                                if (num > 0) {
                                    undo(file, tail, commands, &commands_position, num);
                                } else {
                                    redo(file, tail, commands, &commands_position, -num);
                                }
                            }

                            print(ind1, ind2, *file, *tail);
                            getchar();
                            i = 0; flag = 0; ind1 = 0; ind2 = 0; num_undo = 0; num_redo = 0; num = 0;
                        }
                    }
                } else {
                    if (flag == 1) {
                        if (i == 0) {
                            ind2 = command - '0';
                            i++;
                        } else {
                            ind2 = 10 * ind2;
                            ind2 = ind2 + (command - '0');
                        }
                    } else {
                        if (i == 0) {
                            ind1 = command - '0';
                            i++;
                        } else {
                            ind1 = 10 * ind1;
                            ind1 = ind1 + (command - '0');
                        }
                    }
                }
            }
        } else {
            num = num_undo - num_redo;
            if (num != 0) {
                if (num > 0) {
                    undo(file, tail, commands, &commands_position, num);
                } else {
                    redo(file, tail, commands, &commands_position, -num);
                }
            }
            end_program = 1;
            break;
        }
    }
}

void input_menu() {
    int i = 0, flag = 0, ind1 = 0, ind2 = 0;
    struct file_list *file = NULL;
    struct file_list *tail;
    struct command_list *commands = NULL;
    char command;

    while (1) {
        command = getchar();
        if (command != 'q') {
            if (command == ',' || command == 'r' || command == 'u') {
                if (command == ',') {
                    flag = 1;
                    i = 0;
                } else {
                    if (command == 'r') {
                        new_minimize_undo_redo(-ind1, &file, &tail, &commands);
                        if (end_program == 1) {
                            break;
                        }
                        i = 0; flag = 0; ind1 = 0;
                    } else {
                        new_minimize_undo_redo(ind1, &file, &tail, &commands);
                        if (end_program == 1) {
                            break;
                        }
                        i = 0; flag = 0; ind1 = 0;
                    }
                }
            } else {
                if (command == 'c' || command == 'd' || command == 'p') {
                    if (command == 'c') {
                        change(ind1, ind2, &file, &tail, &commands);
                        getchar();
                        i = 0; flag = 0; ind1 = 0; ind2 = 0;
                    } else {
                        if (command == 'd') {
                            delete(ind1, ind2, &file, &tail, &commands);
                            getchar();
                            i = 0; flag = 0; ind1 = 0; ind2 = 0;
                        } else {
                            print(ind1, ind2, file, tail);
                            getchar();
                            i = 0; flag = 0; ind1 = 0; ind2 = 0;
                        }
                    }
                } else {
                    if (flag == 1) {
                        if (i == 0) {
                            ind2 = command - '0';
                            i++;
                        } else {
                            ind2 = 10 * ind2;
                            ind2 = ind2 + (command - '0');
                        }
                    } else {
                        if (i == 0) {
                            ind1 = command - '0';
                            i++;
                        } else {
                            ind1 = 10 * ind1;
                            ind1 = ind1 + (command - '0');
                        }
                    }
                }
            }
        } else {
            break;
        }
    }
}

int main() {
    input_menu();
    return 0;
}