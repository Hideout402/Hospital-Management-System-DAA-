#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "../include/models/Department.h"
#include "../include/models/Doctor.h"
#include "../include/dsa/DepartmentTree.h"
#include "../include/dsa/DoctorList.h"
#include "../include/dsa/DoctorBST.h"
#include "../include/services/DepartmentService.h"
#include "../include/services/DoctorService.h"

using namespace std;

void testDepartmentTree()
{
    cout << "\n=== Testing DepartmentTree (General / N-ary Tree) ===" << endl;

    vector<Department> depts = {
        {100, "Clinical Services", 0, "Top clinical division", "Tower A"},
        {1, "Cardiology", 100, "Heart care", "Block A, 2nd Floor"},
        {10, "Interventional Cardiology", 1, "Cath Lab", "Block A, Cath Lab"},
        {11, "Electrophysiology", 1, "Arrhythmia", "Block A, 2nd Floor"},
        {2, "Neurology", 100, "Brain & spine", "Block A, 3rd Floor"},
        {200, "Surgical Services", 0, "Surgical division", "Tower B"},
        {3, "Orthopedics", 200, "Joints & bones", "Tower B, 4th Floor"}
    };

    DepartmentTree tree;
    tree.buildFromList(depts);

    // Test findNode
    assert(tree.findNode(100) != nullptr);
    assert(tree.findNode(1) != nullptr);
    assert(tree.findNode(10) != nullptr);
    assert(tree.findNode(999) == nullptr);
    cout << "PASS: DepartmentTree findNode successfully locates existing nodes and handles missing IDs." << endl;

    // Test breadcrumb path
    vector<string> path = tree.getBreadcrumbPath(10);
    assert(path.size() == 3);
    assert(path[0] == "Clinical Services");
    assert(path[1] == "Cardiology");
    assert(path[2] == "Interventional Cardiology");
    cout << "PASS: DepartmentTree getBreadcrumbPath computes correct root-to-leaf hierarchy." << endl;

    // Test subtree collection
    vector<int> cardioSubtree = tree.getSubtreeDepartmentIds(1);
    assert(cardioSubtree.size() == 3); // 1, 10, 11
    assert(find(cardioSubtree.begin(), cardioSubtree.end(), 1) != cardioSubtree.end());
    assert(find(cardioSubtree.begin(), cardioSubtree.end(), 10) != cardioSubtree.end());
    assert(find(cardioSubtree.begin(), cardioSubtree.end(), 11) != cardioSubtree.end());
    cout << "PASS: DepartmentTree getSubtreeDepartmentIds collects all descendant department IDs recursively." << endl;

    // Test cycle detection
    assert(tree.isDescendant(100, 10) == true);
    assert(tree.isDescendant(1, 10) == true);
    assert(tree.isDescendant(10, 100) == false);
    cout << "PASS: DepartmentTree isDescendant prevents cyclical parent assignment." << endl;

    // Test recursive search
    vector<Department> searchResults = tree.searchByName("cardio");
    assert(searchResults.size() >= 2); // Cardiology, Interventional Cardiology
    cout << "PASS: DepartmentTree searchByName traverses tree recursively to find matching units." << endl;

    // Test JSON hierarchy
    nlohmann::json hierarchy = tree.getHierarchyJson();
    assert(hierarchy.is_array());
    assert(hierarchy.size() == 2); // Clinical Services, Surgical Services
    cout << "PASS: DepartmentTree getHierarchyJson produces nested N-ary tree representation." << endl;
}

void testDoctorDoublyLinkedList()
{
    cout << "\n=== Testing DoctorDoublyLinkedList (Doubly Linked List) ===" << endl;

    DoctorDoublyLinkedList roster;

    Doctor d1(1, "Dr. Tariq Mahmood", "Cardiologist", 1, {}, 4000.0, "0301-1111", "", "", "", 16, "Available");
    Doctor d2(2, "Dr. Sarah Johnson", "Pediatrician", 4, {}, 2500.0, "0302-2222", "", "", "", 10, "Available");
    Doctor d3(3, "Dr. Zainab Bilal", "Neurologist", 2, {}, 4500.0, "0303-3333", "", "", "", 12, "In Surgery");

    roster.push_back(d1);
    roster.push_back(d2);
    roster.push_back(d3);

    assert(roster.size() == 3);

    // Forward traversal
    vector<Doctor> forwardList = roster.toVectorForward();
    assert(forwardList.size() == 3);
    assert(forwardList[0].id == 1);
    assert(forwardList[1].id == 2);
    assert(forwardList[2].id == 3);
    cout << "PASS: DoctorDoublyLinkedList forward traversal (Head -> Tail) preserves insertion order." << endl;

    // Backward traversal
    vector<Doctor> backwardList = roster.toVectorBackward();
    assert(backwardList.size() == 3);
    assert(backwardList[0].id == 3);
    assert(backwardList[1].id == 2);
    assert(backwardList[2].id == 1);
    cout << "PASS: DoctorDoublyLinkedList backward traversal (Tail -> Head) correctly traverses reverse pointers." << endl;

    // Shift rotation / Next and Prev on-call
    const Doctor* nextOf1 = roster.getNextDoctor(1);
    assert(nextOf1 != nullptr && nextOf1->id == 2);

    const Doctor* nextOf3 = roster.getNextDoctor(3, true); // Circular wrap
    assert(nextOf3 != nullptr && nextOf3->id == 1);

    const Doctor* prevOf2 = roster.getPrevDoctor(2);
    assert(prevOf2 != nullptr && prevOf2->id == 1);

    const Doctor* prevOf1 = roster.getPrevDoctor(1, true); // Circular wrap
    assert(prevOf1 != nullptr && prevOf1->id == 3);
    cout << "PASS: DoctorDoublyLinkedList bi-directional rotation (next / prev) functions correctly." << endl;

    // Removal with O(1) relinking
    bool removed = roster.remove(2);
    assert(removed);
    assert(roster.size() == 2);
    assert(roster.findForward(2) == nullptr);
    assert(roster.getNextDoctor(1)->id == 3);
    assert(roster.getPrevDoctor(3)->id == 1);
    cout << "PASS: DoctorDoublyLinkedList remove properly splices out node and re-links prev and next pointers." << endl;
}

void testDoctorBST()
{
    cout << "\n=== Testing DoctorBST (Binary Search Tree) ===" << endl;

    DoctorBST bst;

    Doctor d5(5, "Dr. E", "Cardiology", 1);
    Doctor d2(2, "Dr. B", "Neurology", 2);
    Doctor d8(8, "Dr. H", "Orthopedics", 3);
    Doctor d1(1, "Dr. A", "Pediatrics", 4);
    Doctor d3(3, "Dr. C", "Cardiology", 1);
    Doctor d7(7, "Dr. G", "Dermatology", 5);

    bst.insert(d5);
    bst.insert(d2);
    bst.insert(d8);
    bst.insert(d1);
    bst.insert(d3);
    bst.insert(d7);

    // Search O(log n)
    Doctor* found = bst.search(3);
    assert(found != nullptr && found->name == "Dr. C");
    assert(bst.search(99) == nullptr);
    cout << "PASS: DoctorBST search finds nodes in logarithmic time." << endl;

    // In-order traversal gives sorted order
    vector<Doctor> sortedAsc = bst.getSortedDoctorsAscending();
    assert(sortedAsc.size() == 6);
    assert(sortedAsc[0].id == 1);
    assert(sortedAsc[1].id == 2);
    assert(sortedAsc[2].id == 3);
    assert(sortedAsc[3].id == 5);
    assert(sortedAsc[4].id == 7);
    assert(sortedAsc[5].id == 8);
    cout << "PASS: DoctorBST recursive in-order traversal yields doctors sorted by ID ascending." << endl;

    // Reverse in-order traversal
    vector<Doctor> sortedDesc = bst.getSortedDoctorsDescending();
    assert(sortedDesc.size() == 6);
    assert(sortedDesc[0].id == 8);
    assert(sortedDesc[5].id == 1);
    cout << "PASS: DoctorBST reverse in-order traversal yields doctors sorted by ID descending." << endl;

    // Range query
    vector<Doctor> inRange = bst.getDoctorsInRange(2, 6);
    assert(inRange.size() == 3); // IDs: 2, 3, 5
    cout << "PASS: DoctorBST range query locates keys between minId and maxId." << endl;

    // Specialty recursive search
    vector<Doctor> cardioDocs = bst.searchBySpecialty("Cardiology");
    assert(cardioDocs.size() == 2); // Dr. E and Dr. C
    cout << "PASS: DoctorBST searchBySpecialty recursively gathers matching specialists." << endl;
}

void testServiceIntegration()
{
    cout << "\n=== Testing Service Integration & Hierarchy Filtering ===" << endl;

    // Filter doctors with subdepartment hierarchy resolution
    // Department 100 ("Clinical Services") contains Cardiology (1), Neurology (2), Pediatrics (4), Dermatology (5), General Medicine (6)
    vector<Doctor> clinicalDocs = DoctorService::filterDoctors(100, "", "", "", true);
    assert(!clinicalDocs.empty());

    // Sorting by consultation fee descending
    vector<Doctor> sortedByFee = DoctorService::sortDoctors(clinicalDocs, "fee", "desc");
    for (size_t i = 1; i < sortedByFee.size(); ++i)
    {
        assert(sortedByFee[i].consultationFee <= sortedByFee[i - 1].consultationFee);
    }
    cout << "PASS: DoctorService integrates with DepartmentService hierarchy and sorts via Merge Sort." << endl;
}

int main()
{
    cout << "Running Doctor & Department DSA Unit Tests..." << endl;
    testDepartmentTree();
    testDoctorDoublyLinkedList();
    testDoctorBST();
    testServiceIntegration();
    cout << "\nALL DOCTOR & DEPARTMENT DSA TESTS PASSED SUCCESSFULLY!" << endl;
    return 0;
}
