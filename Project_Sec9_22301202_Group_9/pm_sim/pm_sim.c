//part 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MAX 64  // Maximum 64 processes

// Process states
#define RUNNING 1
#define BLOCKED 2
#define ZOMBIE 3
#define DEAD 4

//part 2 (process structure -Simply Array Approach)
// Process Control Block
typedef struct {
    int pid;           // Process ID
    int ppid;          // Parent PID
    int state;         // 1=RUNNING, 2=BLOCKED, 3=ZOMBIE, 4=DEAD
    int exit_status;   // Exit code
    int children[10];  // Array of child PIDs (max 10 children)
    int child_count;   // How many children
} PCB;

// Global process table
PCB table[MAX];
int process_count = 1;  // Start with 1 process (PID 1)
int next_pid = 2;       // Next available PID

// Mutex for synchronization
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// For monitor thread
pthread_cond_t monitor_cond = PTHREAD_COND_INITIALIZER;
int table_changed = 0;
FILE* snapshot = NULL;

//part 3 (Helper function)
// Find process by PID
PCB* find(int pid) {
    if (pid < 1 || pid >= next_pid) return NULL;
    return &table[pid - 1];
}

// Add child to parent
void add_child(int parent_pid, int child_pid) {
    PCB* p = find(parent_pid);
    if (p && p->child_count < 10) {
        p->children[p->child_count++] = child_pid;
    }
}

// Remove child from parent
void remove_child(int parent_pid, int child_pid) {
    PCB* p = find(parent_pid);
    if (!p) return;
    
    for (int i = 0; i < p->child_count; i++) {
        if (p->children[i] == child_pid) {
            for (int j = i; j < p->child_count - 1; j++) {
                p->children[j] = p->children[j + 1];
            }
            p->child_count--;
            return;
        }
    }
}

// Check if parent has this child
int has_child(int parent_pid, int child_pid) {
    PCB* p = find(parent_pid);
    if (!p) return 0;
    
    for (int i = 0; i < p->child_count; i++) {
        if (p->children[i] == child_pid) return 1;
    }
    return 0;
}

// Get state as string
char* state_str(int state) {
    if (state == RUNNING) return "RUNNING";
    if (state == BLOCKED) return "BLOCKED";
    if (state == ZOMBIE) return "ZOMBIE";
    return "TERMINATED";
}

//part 4 (pm_ps() -print Process Table)
void pm_ps() {
    pthread_mutex_lock(&lock);
    
    printf("\n%-8s %-8s %-12s %-12s\n", "PID", "PPID", "STATE", "EXIT_STATUS");
    
    for (int i = 0; i < process_count; i++) {
        PCB* p = &table[i];
        if (p->state != DEAD) {  // Don't show dead processes
            if (p->state == ZOMBIE) {
                printf("%-8d %-8d %-12s %-12d\n", p->pid, p->ppid, state_str(p->state), p->exit_status);
            } else {
                printf("%-8d %-8d %-12s %-12s\n", p->pid, p->ppid, state_str(p->state), "-");
            }
        }
    }
    
    pthread_mutex_unlock(&lock);
}

// part 5 (pm_fork()-create process
int pm_fork(int parent_pid) {
    pthread_mutex_lock(&lock);
    
    // Check if table is full
    if (process_count >= MAX) {
        pthread_mutex_unlock(&lock);
        return -1;
    }
    
    // Find parent
    PCB* parent = find(parent_pid);
    if (!parent || parent->state != RUNNING) {
        pthread_mutex_unlock(&lock);
        return -1;
    }
    
    // Create child
    int child_pid = next_pid++;
    PCB* child = &table[process_count];
    child->pid = child_pid;
    child->ppid = parent_pid;
    child->state = RUNNING;
    child->exit_status = -1;
    child->child_count = 0;
    
    process_count++;
    
    // Add to parent's children
    add_child(parent_pid, child_pid);
    
    // Notify monitor
    table_changed = 1;
    pthread_cond_signal(&monitor_cond);
    
    pthread_mutex_unlock(&lock);
    return child_pid;
}

//part 6 (pm_exit() - Terminate Process

int pm_exit(int pid, int status) {
    pthread_mutex_lock(&lock);
    
    PCB* p = find(pid);
    if (!p) {
        pthread_mutex_unlock(&lock);
        return -1;
    }
    
    // Change to ZOMBIE
    p->state = ZOMBIE;
    p->exit_status = status;
    
    // Notify monitor
    table_changed = 1;
    pthread_cond_signal(&monitor_cond);
    
    pthread_mutex_unlock(&lock);
    return 0;
}


//part 7 (pm_wait() -Simple Wait

int pm_wait(int parent_pid, int child_pid) {
    pthread_mutex_lock(&lock);
    
    PCB* parent = find(parent_pid);
    if (!parent || parent->state != RUNNING) {
        pthread_mutex_unlock(&lock);
        return -1;
    }
    
    // Look for a zombie child
    for (int i = 0; i < parent->child_count; i++) {
        int child_id = parent->children[i];
        PCB* child = find(child_id);
        
        if (child && child->state == ZOMBIE) {
            if (child_pid == -1 || child_pid == child_id) {
                // Found zombie child - reap it
                int status = child->exit_status;
                remove_child(parent_pid, child_id);
                child->state = DEAD;  // Mark as dead
                pthread_mutex_unlock(&lock);
                return status;
            }
        }
    }
    
    // No zombie found
    pthread_mutex_unlock(&lock);
    return -1;
}

//part 8 pm_kill() -Kill process
int pm_kill(int pid) {
    pthread_mutex_lock(&lock);
    
    PCB* p = find(pid);
    if (!p) {
        pthread_mutex_unlock(&lock);
        return -1;
    }
    
    // Kill process (make it zombie with status 9)
    p->state = ZOMBIE;
    p->exit_status = 9;
    
    // Notify monitor
    table_changed = 1;
    pthread_cond_signal(&monitor_cond);
    
    pthread_mutex_unlock(&lock);
    return 0;
}

//part 9 (Monitor Thread (simple)
void write_snapshot() {
    fprintf(snapshot, "\nProcess Table Snapshot:\n");
    fprintf(snapshot, "%-8s %-8s %-12s %-12s\n", "PID", "PPID", "STATE", "EXIT_STATUS");
    
    for (int i = 0; i < process_count; i++) {
        PCB* p = &table[i];
        if (p->state != DEAD) {
            if (p->state == ZOMBIE) {
                fprintf(snapshot, "%-8d %-8d %-12s %-12d\n", p->pid, p->ppid, state_str(p->state), p->exit_status);
            } else {
                fprintf(snapshot, "%-8d %-8d %-12s %-12s\n", p->pid, p->ppid, state_str(p->state), "-");
            }
        }
    }
    fflush(snapshot);
}

void* monitor(void* arg) {
    snapshot = fopen("snapshots.txt", "w");
    
    pthread_mutex_lock(&lock);
    write_snapshot();
    
    while (1) {
        while (!table_changed) {
            pthread_cond_wait(&monitor_cond, &lock);
        }
        table_changed = 0;
        write_snapshot();
    }
    
    pthread_mutex_unlock(&lock);
    return NULL;
}

//part 10 Worker Thread (Simple Script Reader)
void* worker(void* arg) {
    char* filename = (char*)arg;
    FILE* file = fopen(filename, "r");
    
    if (!file) {
        printf("Cannot open %s\n", filename);
        return NULL;
    }
    
    char line[100];
    
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;
        
        char cmd[20];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "fork") == 0) {
            int parent;
            sscanf(line, "%*s %d", &parent);
            printf("Thread %s: fork %d\n", filename, parent);
            pm_fork(parent);
        }
        else if (strcmp(cmd, "exit") == 0) {
            int pid, status;
            sscanf(line, "%*s %d %d", &pid, &status);
            printf("Thread %s: exit %d %d\n", filename, pid, status);
            pm_exit(pid, status);
        }
        else if (strcmp(cmd, "wait") == 0) {
            int parent, child;
            sscanf(line, "%*s %d %d", &parent, &child);
            printf("Thread %s: wait %d %d\n", filename, parent, child);
            int result = pm_wait(parent, child);
            printf("Thread %s: wait returned %d\n", filename, result);
        }
        else if (strcmp(cmd, "kill") == 0) {
            int pid;
            sscanf(line, "%*s %d", &pid);
            printf("Thread %s: kill %d\n", filename, pid);
            pm_kill(pid);
        }
        else if (strcmp(cmd, "sleep") == 0) {
            int ms;
            sscanf(line, "%*s %d", &ms);
            usleep(ms * 1000);
        }
        else if (strcmp(cmd, "ps") == 0) {
            pm_ps();
        }
    }
    
    fclose(file);
    return NULL;
}

//part 11 (Main Function)

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s script1.txt script2.txt ...\n", argv[0]);
        return 1;
    }
    
    // Initialize process table
    memset(table, 0, sizeof(table));
    
    // Create init process (PID=1)
    table[0].pid = 1;
    table[0].ppid = 0;
    table[0].state = RUNNING;
    table[0].exit_status = -1;
    table[0].child_count = 0;
    process_count = 1;
    next_pid = 2;
    
    printf("=== Process Manager Started ===\n");
    printf("Init process created (PID=1)\n");
    pm_ps();
    
    // Start monitor thread
    pthread_t monitor_thread;
    pthread_create(&monitor_thread, NULL, monitor, NULL);
    
    // Create worker threads
    int num_workers = argc - 1;
    pthread_t workers[num_workers];
    
    for (int i = 0; i < num_workers; i++) {
        pthread_create(&workers[i], NULL, worker, argv[i + 1]);
    }
    
    // Wait for workers to finish
    for (int i = 0; i < num_workers; i++) {
        pthread_join(workers[i], NULL);
    }
    
    printf("\n=== All workers finished ===\n");
    
    // Note: Monitor thread keeps running (for simplicity)
    // In full version, you'd stop it properly
    
    return 0;
}
