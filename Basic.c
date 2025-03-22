#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STR_LEN 100
#define INITIAL_CAPACITY 10

// Struct definitions
typedef struct {
    int patientId;
    char name[MAX_STR_LEN];
    int age;
    char gender[MAX_STR_LEN];
    char contact[MAX_STR_LEN];
    char address[MAX_STR_LEN];
    char password[MAX_STR_LEN];
} Patient;

typedef struct {
    int doctorId;
    char name[MAX_STR_LEN];
    char department[MAX_STR_LEN];
    double consultationFee;
    int availableSlots;
    char password[MAX_STR_LEN];
} Doctor;

typedef struct {
    int patientId;
    int doctorId;
} Appointment;

typedef struct {
    int billId;
    int patientId;
    int doctorId;
    double consultationFee;
    double testCost;
    double medicineCost;
    double total;
    char date[MAX_STR_LEN];
} Bill;

typedef struct {
    char name[MAX_STR_LEN];
    double cost;
} TestOption;

// Global variables
int patientIdCounter = 0;
int doctorIdCounter = 0;
int billCounter = 0;


Patient* patients = NULL;
int patientCount = 0, patientCapacity = 0;
Doctor* doctors = NULL;
int doctorCount = 0, doctorCapacity = 0;

Appointment* appointments = NULL;
int appointmentCount = 0, appointmentCapacity = 0;

Bill* bills = NULL;
int billCount = 0, billCapacity = 0;

// Test options per department
TestOption testOptions[5][2] = {
    {{"Blood Test", 150}, {"Urine Test", 100}},  // General Medicine
    {{"ECG", 300}, {"弁Echocardiogram", 500}},     // Cardiology
    {{"X-Ray", 200}, {"MRI", 800}},              // Orthopedics
    {{"Skin Test", 100}, {"Patch Test", 120}},   // Dermatology
    {{"EEG", 350}, {"CT Scan", 600}}             // Neurology
};
char* departments[] = {"General Medicine", "Cardiology", "Orthopedics", "Dermatology", "Neurology"};

// Function prototypes (added to fix warnings)
void resizeArray(void** array, int* capacity, size_t elementSize);
void getCurrentDate(char* date);
void addPatient(Patient p);
void displayPatient(Patient p);
void savePatients(void);
void loadPatients(void);
int patientLogin(void);
void patientRegistration(void);
void addDoctor(Doctor d);
void displayDoctor(Doctor d);
void saveDoctors(void);
void loadDoctors(void);
void initializeDoctors(void);
void addAppointment(Appointment a);
void saveAppointments(void);
void loadAppointments(void);
void addBill(Bill b);
void displayBill(Bill b);
void makeAppointment(int patientId);
void viewBills(int patientId);
void patientMenu(void);
int doctorLogin(void);
void doctorMenu(void);
void mainMenu(void);

// Utility functions
void resizeArray(void** array, int* capacity, size_t elementSize) {
    if (*capacity == 0) *capacity = INITIAL_CAPACITY;
    else *capacity *= 2;
    *array = realloc(*array, *capacity * elementSize);
    if (!*array) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
}

void getCurrentDate(char* date) {
    time_t now = time(NULL);
    char* dt = ctime(&now);
    dt[strlen(dt) - 1] = '\0'; // Remove newline
    strcpy(date, dt);
}

// Patient functions
void addPatient(Patient p) {
    if (patientCount >= patientCapacity) resizeArray((void**)&patients, &patientCapacity, sizeof(Patient));
    patients[patientCount++] = p;
}

void displayPatient(Patient p) {
    printf("Patient ID: %d\nName: %s\nAge: %d\nGender: %s\nContact: %s\nAddress: %s\n",
           p.patientId, p.name, p.age, p.gender, p.contact, p.address);
}

void savePatients() {
    FILE* out = fopen("patients.txt", "w");
    if (!out) {
        printf("Error writing patients.txt\n");
        return;
    }
    for (int i = 0; i < patientCount; i++) {
        fprintf(out, "%d|%s|%d|%s|%s|%s|%s\n",
                patients[i].patientId, patients[i].name, patients[i].age,
                patients[i].gender, patients[i].contact, patients[i].address, patients[i].password);
    }
    fclose(out);
}

void loadPatients() {
    FILE* in = fopen("patients.txt", "r");
    if (!in) return;
    char line[512];
    while (fgets(line, sizeof(line), in)) {
        if (strlen(line) <= 1) continue;
        Patient p;
        sscanf(line, "%d|%[^|]|%d|%[^|]|%[^|]|%[^|]|%s",
               &p.patientId, p.name, &p.age, p.gender, p.contact, p.address, p.password);
        addPatient(p);
        if (p.patientId > patientIdCounter) patientIdCounter = p.patientId;
    }
    fclose(in);
}

int patientLogin() {
    int id;
    char pass[MAX_STR_LEN];
    printf("Enter your Patient ID (or 0 to register): ");
    scanf("%d", &id);
    if (id == 0) {
        patientRegistration();
        return patients[patientCount - 1].patientId;
    }
    for (int i = 0; i < patientCount; i++) {
        if (patients[i].patientId == id) {
            printf("Enter your password: ");
            scanf("%s", pass);
            if (strcmp(pass, patients[i].password) == 0) {
                printf("Welcome, %s!\n", patients[i].name);
                return patients[i].patientId;
            } else {
                printf("Incorrect password. Try again.\n");
                return patientLogin();
            }
        }
    }
    printf("Patient ID not found. Please register.\n");
    patientRegistration();
    return patients[patientCount - 1].patientId;
}

void patientRegistration() {
    Patient p;
    p.patientId = ++patientIdCounter;
    printf("Enter Patient Name: ");
    getchar(); // Clear buffer
    fgets(p.name, MAX_STR_LEN, stdin);
    p.name[strcspn(p.name, "\n")] = 0;
    printf("Enter Age: ");
    scanf("%d", &p.age);
    printf("Enter Gender: ");
    scanf("%s", p.gender);
    printf("Enter Contact: ");
    scanf("%s", p.contact);
    printf("Enter Address: ");
    getchar(); // Clear buffer
    fgets(p.address, MAX_STR_LEN, stdin);
    p.address[strcspn(p.address, "\n")] = 0;
    printf("Set a Password: ");
    scanf("%s", p.password);
    addPatient(p);
    printf("Registration successful. Your Patient ID is: %d\n", p.patientId);
    savePatients();
}

// Doctor functions
void addDoctor(Doctor d) {
    if (doctorCount >= doctorCapacity) resizeArray((void**)&doctors, &doctorCapacity, sizeof(Doctor));
    doctors[doctorCount++] = d;
}

void displayDoctor(Doctor d) {
    printf("Doctor ID: %d | Name: %s | Dept: %s | Fee: %.2f | Slots: %d\n",
           d.doctorId, d.name, d.department, d.consultationFee, d.availableSlots);
}

void saveDoctors() {
    FILE* out = fopen("doctors.txt", "w");
    if (!out) {
        printf("Error writing doctors.txt\n");
        return;
    }
    for (int i = 0; i < doctorCount; i++) {
        fprintf(out, "%d|%s|%s|%.2f|%d|%s\n",
                doctors[i].doctorId, doctors[i].name, doctors[i].department,
                doctors[i].consultationFee, doctors[i].availableSlots, doctors[i].password);
    }
    fclose(out);
}

void loadDoctors() {
    FILE* in = fopen("doctors.txt", "r");
    if (!in) return;
    char line[512];
    while (fgets(line, sizeof(line), in)) {
        if (strlen(line) <= 1) continue;
        Doctor d;
        sscanf(line, "%d|%[^|]|%[^|]|%lf|%d|%s",
               &d.doctorId, d.name, d.department, &d.consultationFee, &d.availableSlots, d.password);
        addDoctor(d);
        if (d.doctorId > doctorIdCounter) doctorIdCounter = d.doctorId;
    }
    fclose(in);
}

void initializeDoctors() {
    if (doctorCount > 0) return;
    Doctor d1 = {++doctorIdCounter, "Dr. Rajesh Kumar", "General Medicine", 300, 5, "doc123"};
    Doctor d2 = {++doctorIdCounter, "Dr. Suresh Singh", "General Medicine", 300, 5, "doc123"};
    Doctor d3 = {++doctorIdCounter, "Dr. Amit Patel", "Cardiology", 500, 3, "doc123"};
    Doctor d4 = {++doctorIdCounter, "Dr. Ramesh Gupta", "Cardiology", 500, 3, "doc123"};
    Doctor d5 = {++doctorIdCounter, "Dr. Vikram Sharma", "Orthopedics", 400, 4, "doc123"};
    Doctor d6 = {++doctorIdCounter, "Dr. Manoj Verma", "Orthopedics", 400, 4, "doc123"};
    Doctor d7 = {++doctorIdCounter, "Dr. Priya Reddy", "Dermatology", 350, 4, "doc123"};
    Doctor d8 = {++doctorIdCounter, "Dr. Anjali Mehta", "Dermatology", 350, 4, "doc123"};
    Doctor d9 = {++doctorIdCounter, "Dr. Sunil Desai", "Neurology", 600, 3, "doc123"};
    Doctor d10 = {++doctorIdCounter, "Dr. Arvind Joshi", "Neurology", 600, 3, "doc123"};
    addDoctor(d1); addDoctor(d2); addDoctor(d3); addDoctor(d4); addDoctor(d5);
    addDoctor(d6); addDoctor(d7); addDoctor(d8); addDoctor(d9); addDoctor(d10);
    saveDoctors();
}

// Appointment functions
void addAppointment(Appointment a) {
    if (appointmentCount >= appointmentCapacity) resizeArray((void**)&appointments, &appointmentCapacity, sizeof(Appointment));
    appointments[appointmentCount++] = a;
}

void saveAppointments() {
    FILE* out = fopen("appointments.txt", "w");
    if (!out) return;
    for (int i = 0; i < appointmentCount; i++) {
        fprintf(out, "%d|%d\n", appointments[i].patientId, appointments[i].doctorId);
    }
    fclose(out);
}

void loadAppointments() {
    FILE* in = fopen("appointments.txt", "r");
    if (!in) return;
    char line[512];
    while (fgets(line, sizeof(line), in)) {
        if (strlen(line) <= 1) continue;
        Appointment a;
        sscanf(line, "%d|%d", &a.patientId, &a.doctorId);
        addAppointment(a);
    }
    fclose(in);
}

// Bill functions
void addBill(Bill b) {
    if (billCount >= billCapacity) resizeArray((void**)&bills, &billCapacity, sizeof(Bill));
    bills[billCount++] = b;
}

void displayBill(Bill b) {
    printf("\nBill ID: %d\nPatient ID: %d\nDoctor ID: %d\nDate: %s\nConsultation Fee: %.2f\nTest Cost: %.2f\nMedicine Cost: %.2f\nTotal Bill: %.2f\n",
           b.billId, b.patientId, b.doctorId, b.date, b.consultationFee, b.testCost, b.medicineCost, b.total);
}

// Patient menu functions
void makeAppointment(int patientId) {
    printf("\nSelect Department:\n1. General Medicine\n2. Cardiology\n3. Orthopedics\n4. Dermatology\n5. Neurology\nEnter choice: ");
    int depChoice;
    scanf("%d", &depChoice);
    if (depChoice < 1 || depChoice > 5) {
        printf("Invalid department choice.\n");
        return;
    }
    char* department = departments[depChoice - 1];

    int deptDoctorCount = 0;
    for (int i = 0; i < doctorCount; i++) {
        if (strcmp(doctors[i].department, department) == 0) deptDoctorCount++;
    }
    if (deptDoctorCount == 0) {
        printf("No doctors available in %s.\n", department);
        return;
    }
    printf("\nAvailable Doctors in %s:\n", department);
    int index = 1;
    for (int i = 0; i < doctorCount; i++) {
        if (strcmp(doctors[i].department, department) == 0) {
            printf("%d. %s (ID: %d, Slots: %d, Fee: %.2f)\n",
                   index++, doctors[i].name, doctors[i].doctorId, doctors[i].availableSlots, doctors[i].consultationFee);
        }
    }
    printf("Select a doctor (enter number): ");
    int docChoice;
    scanf("%d", &docChoice);
    if (docChoice < 1 || docChoice > deptDoctorCount) {
        printf("Invalid doctor selection.\n");
        return;
    }
    Doctor* selectedDoctor = NULL;
    int count = 0;
    for (int i = 0; i < doctorCount; i++) {
        if (strcmp(doctors[i].department, department) == 0) {
            count++;
            if (count == docChoice) {
                selectedDoctor = &doctors[i];
                break;
            }
        }
    }
    if (selectedDoctor->availableSlots <= 0) {
        printf("No available slots for Dr. %s.\n", selectedDoctor->name);
        return;
    }
    selectedDoctor->availableSlots--;
    saveDoctors();
    Appointment app = {patientId, selectedDoctor->doctorId};
    addAppointment(app);
    saveAppointments();
    printf("Appointment booked with Dr. %s.\n", selectedDoctor->name);
}

void viewBills(int patientId) {
    printf("\n--- Your Bills ---\n");
    int found = 0;
    for (int i = 0; i < billCount; i++) {
        if (bills[i].patientId == patientId) {
            displayBill(bills[i]);
            found = 1;
        }
    }
    if (!found) printf("No bills available.\n");
}

void patientMenu() {
    int pid = patientLogin();
    int choice;
    do {
        printf("\nPatient Menu:\n1. Make Appointment and Consultation\n2. View My Bills\n3. Track My Details\n4. Logout\nEnter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: makeAppointment(pid); break;
            case 2: viewBills(pid); break;
            case 3:
                for (int i = 0; i < patientCount; i++) {
                    if (patients[i].patientId == pid) {
                        displayPatient(patients[i]);
                        break;
                    }
                }
                break;
            case 4: printf("Logging out...\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 4);
}

// Doctor menu functions
int doctorLogin() {
    int id;
    char pass[MAX_STR_LEN];
    printf("Enter your Doctor ID: ");
    scanf("%d", &id);
    for (int i = 0; i < doctorCount; i++) {
        if (doctors[i].doctorId == id) {
            printf("Enter your password: ");
            scanf("%s", pass);
            if (strcmp(pass, doctors[i].password) == 0) {
                printf("Welcome Dr. %s!\n", doctors[i].name);
                return doctors[i].doctorId;
            } else {
                printf("Incorrect password. Try again.\n");
                return doctorLogin();
            }
        }
    }
    printf("Doctor ID not found.\n");
    return doctorLogin();
}

void doctorMenu() {
    int did = doctorLogin();
    int choice;
    do {
        printf("\nDoctor Menu:\n1. View Appointments\n2. Conduct Consultation\n3. Logout\nEnter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: {
                printf("\nYour Appointments:\n");
                int found = 0;
                for (int i = 0; i < appointmentCount; i++) {
                    if (appointments[i].doctorId == did) {
                        char pname[MAX_STR_LEN] = "";
                        for (int j = 0; j < patientCount; j++) {
                            if (patients[j].patientId == appointments[i].patientId) {
                                strcpy(pname, patients[j].name);
                                break;
                            }
                        }
                        printf("%d. Patient ID: %d (%s)\n", i + 1, appointments[i].patientId, pname);
                        found = 1;
                    }
                }
                if (!found) printf("No appointments found.\n");
                break;
            }
            case 2: {
                printf("Enter the appointment number to conduct consultation: ");
                int appNum;
                scanf("%d", &appNum);
                if (appNum < 1 || appNum > appointmentCount) {
                    printf("Invalid selection.\n");
                    break;
                }
                if (appointments[appNum - 1].doctorId != did) {
                    printf("You cannot conduct consultation for this appointment.\n");
                    break;
                }
                Appointment app = appointments[appNum - 1];

                char department[MAX_STR_LEN];
                double docFee = 0;
                for (int i = 0; i < doctorCount; i++) {
                    if (doctors[i].doctorId == did) {
                        strcpy(department, doctors[i].department);
                        docFee = doctors[i].consultationFee;
                        break;
                    }
                }
                int depIndex = -1;
                for (int i = 0; i < 5; i++) {
                    if (strcmp(department, departments[i]) == 0) {
                        depIndex = i;
                        break;
                    }
                }
                if (depIndex == -1) {
                    printf("No test options available for your department.\n");
                    break;
                }
                printf("Available tests for %s:\n", department);
                for (int i = 0; i < 2; i++) {
                    printf("%d. %s (Price: %.2f)\n", i + 1, testOptions[depIndex][i].name, testOptions[depIndex][i].cost);
                }
                printf("Select a test to conduct (enter number): ");
                int testChoice;
                scanf("%d", &testChoice);
                if (testChoice < 1 || testChoice > 2) {
                    printf("Invalid test selection.\n");
                    break;
                }
                double testCost = testOptions[depIndex][testChoice - 1].cost;

                double medicineCost;
                printf("Enter total medicine cost: ");
                scanf("%lf", &medicineCost);

                char currDate[MAX_STR_LEN];
                getCurrentDate(currDate);

                Bill b = {++billCounter, app.patientId, did, docFee, testCost, medicineCost, docFee + testCost + medicineCost};
                strcpy(b.date, currDate);
                addBill(b);
                printf("Consultation complete. Total Bill: %.2f\n", b.total);
                displayBill(b);

                for (int i = appNum - 1; i < appointmentCount - 1; i++) {
                    appointments[i] = appointments[i + 1];
                }
                appointmentCount--;
                saveAppointments();
                break;
            }
            case 3: printf("Logging out...\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 3);
}

// Main menu
void mainMenu() {
    int roleChoice;
    do {
        printf("\n--- Welcome to Hospital Management System ---\n");
        printf("Are you a:\n1. Patient\n2. Doctor\n0. Exit\nEnter your choice: ");
        scanf("%d", &roleChoice);
        switch (roleChoice) {
            case 1: patientMenu(); break;
            case 2: doctorMenu(); break;
            case 0: printf("Exiting system.\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (roleChoice != 0);
}

int main() {
    loadPatients();
    loadDoctors();
    loadAppointments();
    initializeDoctors();
    mainMenu();
    free(patients);
    free(doctors);
    free(appointments);
    free(bills);
    return 0;
}