#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class Shipment {
private:
  int id;
  string source;
  string destination;
  string status;
  string eta;

  static const int MAX_HISTORY = 20;
  string statusHistory[MAX_HISTORY];
  int historyCount;

public:
  Shipment() : id(0), historyCount(0) {}

  Shipment(int i, const string &src, const string &dest,
           const string &expectedArrival)
      : id(i), source(src), destination(dest), status("packed"),
        eta(expectedArrival), historyCount(0) {
    addHistory("packed - Initial status setup");
  }

  int getId() const { return id; }
  string getStatus() const { return status; }
  string getSource() const { return source; }
  string getDestination() const { return destination; }

  void setStatus(const string &newStatus) {
    status = newStatus;
    addHistory(newStatus + " - Status updated");
  }

  void addHistory(const string &historyItem) {
    if (historyCount < MAX_HISTORY) {
      statusHistory[historyCount++] = historyItem;
    } else {
      cout << "Warning: History capacity reached for Shipment ID " << id
           << endl;
    }
  }

  void displayInfo() const {
    cout << "Shipment ID   : " << id << endl;
    cout << "Source        : " << source << endl;
    cout << "Destination   : " << destination << endl;
    cout << "ETA           : " << eta << endl;
    cout << "Status        : " << status << endl;
    cout << "----------------------------------" << endl;
  }

  void displayTimeline() const {
    cout << "\n--- Status Timeline for Shipment ID: " << id << " ---" << endl;
    for (int i = 0; i < historyCount; ++i) {
      cout << " [" << (i + 1) << "] " << statusHistory[i] << endl;
    }
    cout << "---------------------------------------" << endl;
  }

  void saveToFile(ofstream &outFile) const {
    outFile << id << "\n"
            << source << "\n"
            << destination << "\n"
            << eta << "\n"
            << status << "\n"
            << historyCount << "\n";
    for (int i = 0; i < historyCount; ++i) {
      outFile << statusHistory[i] << "\n";
    }
  }

  void loadFromFile(ifstream &inFile) {
    inFile >> id;
    inFile.ignore(10000, '\n');
    getline(inFile, source);
    getline(inFile, destination);
    getline(inFile, eta);
    getline(inFile, status);
    inFile >> historyCount;
    inFile.ignore(10000, '\n');

    for (int i = 0; i < historyCount; ++i) {
      getline(inFile, statusHistory[i]);
    }
  }
};

class SupplyChainSystem {
private:
  static const int MAX_SHIPMENTS = 100;
  Shipment shipments[MAX_SHIPMENTS];
  int shipmentCount;
  string dataFile;

public:
  SupplyChainSystem(const string &filename = "shipments_data.txt")
      : shipmentCount(0), dataFile(filename) {
    loadData();
  }

  ~SupplyChainSystem() { saveData(); }

  void loadData() {
    ifstream inFile(dataFile);
    if (inFile.is_open()) {
      inFile >> shipmentCount;
      if (inFile.fail()) {
        shipmentCount = 0;
      } else {
        inFile.ignore(10000, '\n');
        for (int i = 0; i < shipmentCount; ++i) {
          shipments[i].loadFromFile(inFile);
        }
        cout << "Loaded " << shipmentCount << " shipments from file." << endl;
      }
      inFile.close();
    } else {
      cout << "No existing data file found. Starting fresh." << endl;
    }
  }

  void saveData() const {
    ofstream outFile(dataFile);
    if (outFile.is_open()) {
      outFile << shipmentCount << "\n";
      for (int i = 0; i < shipmentCount; ++i) {
        shipments[i].saveToFile(outFile);
      }
      outFile.close();
    } else {
      cout << "Error: Could not open file to save data." << endl;
    }
  }

  void addShipment(int id, const string &source, const string &destination,
                   const string &eta) {
    if (shipmentCount >= MAX_SHIPMENTS) {
      cout << "Error: System capacity reached. Cannot add more shipments."
           << endl;
      return;
    }

    for (int i = 0; i < shipmentCount; ++i) {
      if (shipments[i].getId() == id) {
        cout << "Error: Shipment with ID " << id << " already exists." << endl;
        return;
      }
    }

    shipments[shipmentCount] = Shipment(id, source, destination, eta);
    shipmentCount++;
    cout << "Shipment added successfully! Default status set to 'packed'."
         << endl;
    saveData();
  }

  void addShipment(int id, const string &source, const string &destination) {
    addShipment(id, source, destination, "TBD (To Be Determined)");
  }

  void trackShipment(int id) const {
    for (int i = 0; i < shipmentCount; ++i) {
      if (shipments[i].getId() == id) {
        cout << "\n--- Tracking Shipment ---" << endl;
        shipments[i].displayInfo();
        shipments[i].displayTimeline();
        return;
      }
    }
    cout << "Error: Shipment with ID " << id << " not found." << endl;
  }

  void trackShipment(const string &source, const string &destination) const {
    bool found = false;
    for (int i = 0; i < shipmentCount; ++i) {
      if (shipments[i].getSource() == source &&
          shipments[i].getDestination() == destination) {
        if (!found) {
          cout << "\n--- Shipments matching Route (" << source << " to "
               << destination << ") ---" << endl;
          found = true;
        }
        shipments[i].displayInfo();
      }
    }
    if (!found) {
      cout << "Error: No shipments found traveling from " << source << " to "
           << destination << "." << endl;
    }
  }

  void viewAllShipments() const {
    if (shipmentCount == 0) {
      cout << "\nNo shipments in the system." << endl;
      return;
    }
    cout << "\n========== All Shipments Dashboard ==========" << endl;
    for (int i = 0; i < shipmentCount; ++i) {
      shipments[i].displayInfo();
    }
    cout << "=============================================" << endl;
  }

  void updateStatus(int id, const string &newStatus) {
    for (int i = 0; i < shipmentCount; ++i) {
      if (shipments[i].getId() == id) {
        if (shipments[i].getStatus() == "delivered") {
          cout << "Shipment is already delivered. Status cannot be changed."
               << endl;
          return;
        }
        shipments[i].setStatus(newStatus);
        cout << "Status updated successfully to '" << newStatus << "'." << endl;
        saveData();
        return;
      }
    }
    cout << "Error: Shipment with ID " << id << " not found." << endl;
  }

  void confirmDelivery(int id) {
    for (int i = 0; i < shipmentCount; ++i) {
      if (shipments[i].getId() == id) {
        shipments[i].setStatus("delivered");
        cout << "Delivery confirmed for Shipment ID " << id << "." << endl;
        saveData();
        return;
      }
    }
    cout << "Error: Shipment with ID " << id << " not found." << endl;
  }
};

void displayMenu() {
  cout << "\n=============================================" << endl;
  cout << "  Logistics & Supply Chain Tracking System" << endl;
  cout << "=============================================" << endl;
  cout << "1. Add Shipment (With ETA)" << endl;
  cout << "2. Add Shipment (Without ETA)" << endl;
  cout << "3. Track Shipment by ID & View Timeline" << endl;
  cout << "4. Track Shipment by Route (Source/Dest)" << endl;
  cout << "5. Update Status" << endl;
  cout << "6. Confirm Delivery" << endl;
  cout << "7. View All Shipments Dashboard" << endl;
  cout << "8. Exit" << endl;
  cout << "Enter your choice: ";
}

int main() {
  SupplyChainSystem system;
  int choice;

  do {
    displayMenu();
    if (!(cin >> choice)) {
      cout << "Invalid input. Please enter a number." << endl;
      cin.clear();
      cin.ignore(10000, '\n');
      continue;
    }

    if (choice == 1) {
      int id;
      string src, dest, eta;
      cout << "Enter Shipment ID: ";
      cin >> id;
      cout << "Enter Source: ";
      cin.ignore(10000, '\n');
      getline(cin, src);
      cout << "Enter Destination: ";
      getline(cin, dest);
      cout << "Enter Estimated Delivery Time (ETA): ";
      getline(cin, eta);

      system.addShipment(id, src, dest, eta);

    } else if (choice == 2) {
      int id;
      string src, dest;
      cout << "Enter Shipment ID: ";
      cin >> id;
      cout << "Enter Source: ";
      cin.ignore(10000, '\n');
      getline(cin, src);
      cout << "Enter Destination: ";
      getline(cin, dest);

      system.addShipment(id, src, dest);

    } else if (choice == 3) {
      int id;
      cout << "Enter Shipment ID to track: ";
      cin >> id;
      system.trackShipment(id);

    } else if (choice == 4) {
      string src, dest;
      cout << "Enter Source: ";
      cin.ignore(10000, '\n');
      getline(cin, src);
      cout << "Enter Destination: ";
      getline(cin, dest);

      system.trackShipment(src, dest);

    } else if (choice == 5) {
      int id;
      int statusChoice;
      cout << "Enter Shipment ID to update: ";
      cin >> id;
      cout << "Select New Status:" << endl;
      cout << "1. packed" << endl;
      cout << "2. dispatched" << endl;
      cout << "3. in transit" << endl;
      cout << "4. out for delivery" << endl;
      cout << "5. delivered" << endl;
      cout << "Enter choice (1-5): ";
      cin >> statusChoice;

      string newStatus;
      switch (statusChoice) {
      case 1:
        newStatus = "packed";
        break;
      case 2:
        newStatus = "dispatched";
        break;
      case 3:
        newStatus = "in transit";
        break;
      case 4:
        newStatus = "out for delivery";
        break;
      case 5:
        newStatus = "delivered";
        break;
      default:
        cout << "Invalid status choice. Update failed." << endl;
        continue;
      }
      system.updateStatus(id, newStatus);

    } else if (choice == 6) {
      int id;
      cout << "Enter Shipment ID to confirm delivery: ";
      cin >> id;
      system.confirmDelivery(id);

    } else if (choice == 7) {
      system.viewAllShipments();

    } else if (choice == 8) {
      cout << "Exiting system. Goodbye!" << endl;

    } else {
      cout << "Invalid choice. Please try again." << endl;
    }
  } while (choice != 8);

  return 0;
}
