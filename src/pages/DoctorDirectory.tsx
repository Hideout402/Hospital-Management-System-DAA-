import { useState, useMemo } from 'react';
import {
  Stethoscope,
  Plus,
  Search,
  RotateCcw,
  Pencil,
  Trash2,
  MapPin,
  Building2,
  Phone,
  Mail,
  UserCheck,
  Clock,
  Activity,
  AlertTriangle,
  X,
  CheckCircle2,
  Award,
  DollarSign,
} from 'lucide-react';
import type { Doctor, DoctorAvailability, DoctorFormData } from '../types/doctor';
import {
  initialDoctors,
  DEPARTMENT_OPTIONS,
  AVAILABILITY_OPTIONS,
} from '../mockdata/doctors';

const emptyDoctorForm: DoctorFormData = {
  name: '',
  specialty: 'Pediatricians',
  department: 'Pediatrics',
  roomNumber: '',
  consultationFee: 2500,
  availability: 'Available',
  phone: '',
  email: '',
  qualification: 'MBBS, FCPS',
  experienceYears: 5,
};

export default function DoctorDirectory() {
  const [doctors, setDoctors] = useState<Doctor[]>(initialDoctors);

  // Filters
  const [searchQuery, setSearchQuery] = useState('');
  const [departmentFilter, setDepartmentFilter] = useState('All Departments');
  const [availabilityFilter, setAvailabilityFilter] = useState<'All' | DoctorAvailability>('All');

  // Modals & Feedback
  const [isModalOpen, setIsModalOpen] = useState(false);
  const [editingDoctor, setEditingDoctor] = useState<Doctor | null>(null);
  const [formData, setFormData] = useState<DoctorFormData>(emptyDoctorForm);
  const [formErrors, setFormErrors] = useState<Record<string, string>>({});

  const [deleteTarget, setDeleteTarget] = useState<Doctor | null>(null);
  const [deletedDoctorBackup, setDeletedDoctorBackup] = useState<Doctor | null>(null);
  const [toastMessage, setToastMessage] = useState<string | null>(null);

  // Instant notification timer helper
  const showToast = (message: string) => {
    setToastMessage(message);
    setTimeout(() => {
      setToastMessage((current) => (current === message ? null : current));
    }, 4000);
  };

  // Filtered doctors
  const filteredDoctors = useMemo(() => {
    const q = searchQuery.trim().toLowerCase();

    return doctors.filter((doctor) => {
      // 1. Search query by name, specialty, room number, or qualification
      let matchesSearch = true;
      if (q) {
        matchesSearch =
          doctor.name.toLowerCase().includes(q) ||
          doctor.specialty.toLowerCase().includes(q) ||
          doctor.roomNumber.toLowerCase().includes(q) ||
          (doctor.qualification ? doctor.qualification.toLowerCase().includes(q) : false);
      }

      // 2. Department filter
      let matchesDepartment = true;
      if (departmentFilter !== 'All Departments') {
        matchesDepartment =
          doctor.department.toLowerCase() === departmentFilter.toLowerCase();
      }

      // 3. Availability filter
      let matchesAvailability = true;
      if (availabilityFilter !== 'All') {
        matchesAvailability = doctor.availability === availabilityFilter;
      }

      return (
        matchesSearch &&
        matchesDepartment &&
        matchesAvailability
      );
    });
  }, [doctors, searchQuery, departmentFilter, availabilityFilter]);

  // Active filters check
  const hasActiveFilters =
    searchQuery.trim() !== '' ||
    departmentFilter !== 'All Departments' ||
    availabilityFilter !== 'All';

  const resetFilters = () => {
    setSearchQuery('');
    setDepartmentFilter('All Departments');
    setAvailabilityFilter('All');
  };

  // Statistics
  const availableCount = doctors.filter((d) => d.availability === 'Available').length;
  const inSurgeryCount = doctors.filter((d) => d.availability === 'In Surgery').length;
  const onLeaveCount = doctors.filter((d) => d.availability === 'On Leave').length;

  // Open Add modal
  const handleOpenAdd = () => {
    setEditingDoctor(null);
    setFormData(emptyDoctorForm);
    setFormErrors({});
    setIsModalOpen(true);
  };

  // Open Edit modal
  const handleOpenEdit = (doctor: Doctor) => {
    setEditingDoctor(doctor);
    setFormData({
      name: doctor.name,
      specialty: doctor.specialty,
      department: doctor.department,
      roomNumber: doctor.roomNumber,
      consultationFee: doctor.consultationFee,
      availability: doctor.availability,
      phone: doctor.phone || '',
      email: doctor.email || '',
      qualification: doctor.qualification || '',
      experienceYears: doctor.experienceYears || 0,
      avatarBg: doctor.avatarBg,
    });
    setFormErrors({});
    setIsModalOpen(true);
  };

  // Validate form
  const validateForm = (): boolean => {
    const errors: Record<string, string> = {};
    if (!formData.name.trim()) {
      errors.name = 'Doctor name is required';
    }
    if (!formData.specialty.trim()) {
      errors.specialty = 'Specialty is required';
    }
    if (!formData.department.trim()) {
      errors.department = 'Department is required';
    }
    if (!formData.roomNumber.trim()) {
      errors.roomNumber = 'Room number is required';
    }
    if (formData.consultationFee === undefined || formData.consultationFee < 0) {
      errors.consultationFee = 'Valid consultation fee in PKR is required';
    }

    setFormErrors(errors);
    return Object.keys(errors).length === 0;
  };

  // Submit Add / Edit
  const handleFormSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (!validateForm()) return;

    if (editingDoctor) {
      // Edit existing doctor
      setDoctors((prev) =>
        prev.map((doc) =>
          doc.id === editingDoctor.id
            ? {
                ...doc,
                ...formData,
                consultationFee: Number(formData.consultationFee),
                experienceYears: Number(formData.experienceYears) || 0,
              }
            : doc
        )
      );
      showToast(`Updated doctor profile for ${formData.name}`);
    } else {
      // Add new doctor
      const newDoctor: Doctor = {
        id: `DOC-${Date.now().toString().slice(-4)}`,
        name: formData.name.trim(),
        specialty: formData.specialty.trim(),
        department: formData.department.trim(),
        roomNumber: formData.roomNumber.trim(),
        consultationFee: Number(formData.consultationFee),
        availability: formData.availability,
        phone: formData.phone?.trim() || '0300-0000000',
        email: formData.email?.trim() || `${formData.name.toLowerCase().replace(/[^a-z]/g, '')}@carepoint.pk`,
        qualification: formData.qualification?.trim() || 'MBBS, FCPS',
        experienceYears: Number(formData.experienceYears) || 5,
        avatarBg: '#dcfce7',
      };
      setDoctors((prev) => [newDoctor, ...prev]);
      showToast(`Added ${newDoctor.name} to the directory`);
    }

    setIsModalOpen(false);
  };

  // Delete flow
  const handleDeleteClick = (doctor: Doctor) => {
    setDeleteTarget(doctor);
  };

  const handleConfirmDelete = () => {
    if (!deleteTarget) return;
    const target = deleteTarget;
    setDeletedDoctorBackup(target);
    setDoctors((prev) => prev.filter((d) => d.id !== target.id));
    setDeleteTarget(null);
    showToast(`Removed ${target.name} from directory`);
  };

  const handleUndoDelete = () => {
    if (deletedDoctorBackup) {
      setDoctors((prev) => [deletedDoctorBackup, ...prev]);
      showToast(`Restored ${deletedDoctorBackup.name}`);
      setDeletedDoctorBackup(null);
    }
  };

  // Helper for Status Badge
  const renderStatusBadge = (status: DoctorAvailability) => {
    switch (status) {
      case 'Available':
        return (
          <span className="inline-flex items-center gap-1.5 px-2.5 py-1 rounded-full text-xs font-semibold bg-[#e4f5ec] text-[#1c6446] border border-[#bfe7d3]">
            <span className="w-1.5 h-1.5 rounded-full bg-[#16a34a] animate-pulse" />
            Available
          </span>
        );
      case 'On Leave':
        return (
          <span className="inline-flex items-center gap-1.5 px-2.5 py-1 rounded-full text-xs font-semibold bg-[#fef3c7] text-[#92400e] border border-[#fde68a]">
            <span className="w-1.5 h-1.5 rounded-full bg-[#d97706]" />
            On Leave
          </span>
        );
      case 'In Surgery':
        return (
          <span className="inline-flex items-center gap-1.5 px-2.5 py-1 rounded-full text-xs font-semibold bg-[#fee2e2] text-[#991b1b] border border-[#fca5a5]">
            <span className="w-1.5 h-1.5 rounded-full bg-[#dc2626]" />
            In Surgery
          </span>
        );
    }
  };

  // Helper for doctor initials
  const getInitials = (name: string) => {
    const clean = name.replace(/^Dr\.\s*/i, '');
    const parts = clean.trim().split(' ');
    if (parts.length === 1) return parts[0].slice(0, 2).toUpperCase();
    return (parts[0][0] + parts[parts.length - 1][0]).toUpperCase();
  };

  return (
    <div className="fade-in max-w-7xl mx-auto space-y-6">
      {/* Toast feedback notification */}
      {toastMessage && (
        <div className="fixed bottom-6 right-6 z-50 flex items-center gap-3 bg-[#173f35] text-white px-4 py-3 rounded-xl shadow-lg border border-[#2b6153] animate-bounce">
          <CheckCircle2 size={18} className="text-[#4ade80]" />
          <span className="text-sm font-medium">{toastMessage}</span>
          {deletedDoctorBackup && (
            <button
              onClick={handleUndoDelete}
              className="ml-2 underline text-xs font-bold text-[#86efac] hover:text-white"
            >
              Undo
            </button>
          )}
        </div>
      )}

      {/* Header section with title and 'Add New Doctor' button */}
      <div className="flex flex-col sm:flex-row sm:items-center sm:justify-between gap-4 pb-2 border-b border-[#e5e7eb]">
        <div>
          <div className="flex items-center gap-2 text-xs font-bold uppercase tracking-wider text-[#2e7d67] mb-1">
            <Stethoscope size={15} />
            Medical Staff Directory
          </div>
          <h1 className="text-2xl sm:text-3xl font-extrabold text-[#143d38] tracking-tight">
            Doctors & Departments
          </h1>
          <p className="mt-1 text-sm text-[#738480]">
            Browse medical specialists, room assignments, consultation fees, and real-time duty status.
          </p>
        </div>

        <button
          onClick={handleOpenAdd}
          className="inline-flex items-center justify-center gap-2 rounded-lg bg-[#176153] hover:bg-[#124e44] text-white px-4 py-2.5 text-sm font-semibold shadow-sm transition"
        >
          <Plus size={18} />
          Add New Doctor
        </button>
      </div>

      {/* Quick Stat Cards */}
      <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
        <div className="bg-white rounded-xl border border-[#e0ebe8] p-4 shadow-sm flex items-center justify-between">
          <div>
            <p className="text-xs text-[#738480] font-medium">Total Doctors</p>
            <p className="text-2xl font-extrabold text-[#143d38] mt-1">{doctors.length}</p>
            <p className="text-[11px] text-[#9aa7a4] mt-0.5">Across all clinics</p>
          </div>
          <div className="w-10 h-10 rounded-xl bg-[#e0f3e9] text-[#2b8063] grid place-items-center">
            <Stethoscope size={20} />
          </div>
        </div>

        <div className="bg-white rounded-xl border border-[#e0ebe8] p-4 shadow-sm flex items-center justify-between">
          <div>
            <p className="text-xs text-[#738480] font-medium">Available Now</p>
            <p className="text-2xl font-extrabold text-[#15803d] mt-1">{availableCount}</p>
            <p className="text-[11px] text-[#9aa7a4] mt-0.5">Ready for OPD</p>
          </div>
          <div className="w-10 h-10 rounded-xl bg-[#dcfce7] text-[#15803d] grid place-items-center">
            <UserCheck size={20} />
          </div>
        </div>

        <div className="bg-white rounded-xl border border-[#e0ebe8] p-4 shadow-sm flex items-center justify-between">
          <div>
            <p className="text-xs text-[#738480] font-medium">In Surgery</p>
            <p className="text-2xl font-extrabold text-[#dc2626] mt-1">{inSurgeryCount}</p>
            <p className="text-[11px] text-[#9aa7a4] mt-0.5">OT active cases</p>
          </div>
          <div className="w-10 h-10 rounded-xl bg-[#fee2e2] text-[#dc2626] grid place-items-center">
            <Activity size={20} />
          </div>
        </div>

        <div className="bg-white rounded-xl border border-[#e0ebe8] p-4 shadow-sm flex items-center justify-between">
          <div>
            <p className="text-xs text-[#738480] font-medium">On Leave</p>
            <p className="text-2xl font-extrabold text-[#d97706] mt-1">{onLeaveCount}</p>
            <p className="text-[11px] text-[#9aa7a4] mt-0.5">Off duty today</p>
          </div>
          <div className="w-10 h-10 rounded-xl bg-[#fef3c7] text-[#d97706] grid place-items-center">
            <Clock size={20} />
          </div>
        </div>
      </div>

      {/* Filter and Search Bar Section */}
      <div className="bg-white rounded-xl border border-[#e0ebe8] p-4 sm:p-5 shadow-sm space-y-3">
        <div className="flex flex-col sm:flex-row items-stretch sm:items-center gap-3">
          {/* Search bar to look up doctors by name */}
          <div className="relative flex-1 min-w-[240px]">
            <Search
              size={17}
              className="absolute left-3.5 top-1/2 -translate-y-1/2 text-[#8d9d99]"
            />
            <input
              type="text"
              aria-label="Search doctors by name"
              placeholder="Search doctors by name..."
              value={searchQuery}
              onChange={(e) => setSearchQuery(e.target.value)}
              className="h-10 w-full rounded-lg border border-[#dce6e3] pl-10 pr-8 text-xs text-[#1f2937] outline-none focus:border-[#4f9a87] focus:ring-2 focus:ring-[#4f9a87]/20 transition"
            />
            {searchQuery && (
              <button
                type="button"
                onClick={() => setSearchQuery('')}
                className="absolute right-2.5 top-1/2 -translate-y-1/2 text-[#9aa7a4] hover:text-[#526360]"
                aria-label="Clear search"
              >
                <X size={15} />
              </button>
            )}
          </div>

          {/* Secondary filter: Departments */}
          <select
            aria-label="Filter by department"
            value={departmentFilter}
            onChange={(e) => setDepartmentFilter(e.target.value)}
            className="h-10 shrink-0 rounded-lg border border-[#dce6e3] bg-white px-3 text-xs text-[#4b635e] font-medium outline-none focus:border-[#4f9a87] focus:ring-2 focus:ring-[#4f9a87]/20 transition cursor-pointer min-w-[150px]"
          >
            {DEPARTMENT_OPTIONS.map((dept) => (
              <option key={dept} value={dept}>
                {dept}
              </option>
            ))}
          </select>

          {/* Secondary filter: Availability */}
          <select
            aria-label="Filter by availability"
            value={availabilityFilter}
            onChange={(e) =>
              setAvailabilityFilter(e.target.value as 'All' | DoctorAvailability)
            }
            className="h-10 shrink-0 rounded-lg border border-[#dce6e3] bg-white px-3 text-xs text-[#4b635e] font-medium outline-none focus:border-[#4f9a87] focus:ring-2 focus:ring-[#4f9a87]/20 transition cursor-pointer min-w-[140px]"
          >
            {AVAILABILITY_OPTIONS.map((avail) => (
              <option key={avail} value={avail}>
                {avail === 'All' ? 'All Availability' : avail}
              </option>
            ))}
          </select>

          {/* Reset / Clear Button */}
          {hasActiveFilters && (
            <button
              type="button"
              onClick={resetFilters}
              className="h-10 px-3 inline-flex items-center gap-1.5 rounded-lg border border-[#dce6e3] bg-white text-xs font-semibold text-[#3b7264] hover:bg-[#eef7f3] transition shrink-0"
            >
              <RotateCcw size={14} />
              Clear
            </button>
          )}
        </div>

        {/* Counter and Active Filter Pills */}
        <div className="flex flex-wrap items-center justify-between gap-2 pt-2 border-t border-[#f0f4f2] text-xs text-[#738480]">
          <div>
            Showing <strong className="text-[#143d38]">{filteredDoctors.length}</strong> of{' '}
            <strong className="text-[#143d38]">{doctors.length}</strong> doctors
          </div>

          {hasActiveFilters && (
            <div className="flex flex-wrap items-center gap-1.5">
              <span className="text-[11px] font-medium text-[#8f9e9a]">Active filters:</span>
              {departmentFilter !== 'All Departments' && (
                <span className="inline-flex items-center gap-1 px-2 py-0.5 rounded-md bg-[#e4f5ec] text-[#287452] text-[11px] font-semibold">
                  Dept: {departmentFilter}
                  <button onClick={() => setDepartmentFilter('All Departments')}>
                    <X size={12} />
                  </button>
                </span>
              )}
              {availabilityFilter !== 'All' && (
                <span className="inline-flex items-center gap-1 px-2 py-0.5 rounded-md bg-[#e4f5ec] text-[#287452] text-[11px] font-semibold">
                  Status: {availabilityFilter}
                  <button onClick={() => setAvailabilityFilter('All')}>
                    <X size={12} />
                  </button>
                </span>
              )}
              {searchQuery && (
                <span className="inline-flex items-center gap-1 px-2 py-0.5 rounded-md bg-[#e4f5ec] text-[#287452] text-[11px] font-semibold">
                  Search: "{searchQuery}"
                  <button onClick={() => setSearchQuery('')}>
                    <X size={12} />
                  </button>
                </span>
              )}
            </div>
          )}
        </div>
      </div>

      {/* Responsive Card Grid showing doctor details */}
      {filteredDoctors.length > 0 ? (
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-5">
          {filteredDoctors.map((doctor) => (
            <article
              key={doctor.id}
              className="bg-white rounded-xl border border-[#e0ebe8] p-5 shadow-sm hover:shadow-md hover:border-[#3e8c79]/40 transition-all duration-200 flex flex-col justify-between"
            >
              <div>
                {/* Top card row: Avatar, Name, and Status Badge */}
                <div className="flex items-start justify-between gap-3">
                  <div className="flex items-center gap-3">
                    <div
                      style={{ backgroundColor: doctor.avatarBg || '#dcfce7' }}
                      className="w-12 h-12 rounded-xl grid place-items-center text-sm font-bold text-[#143d38] shrink-0 shadow-xs border border-white"
                    >
                      {getInitials(doctor.name)}
                    </div>
                    <div>
                      <h3 className="font-bold text-[15px] text-[#143d38] leading-tight">
                        {doctor.name}
                      </h3>
                      <p className="text-xs text-[#738480] mt-0.5">
                        {doctor.qualification || 'MBBS'}
                      </p>
                    </div>
                  </div>

                  <div className="shrink-0">{renderStatusBadge(doctor.availability)}</div>
                </div>

                {/* Specialty Pill */}
                <div className="mt-3.5 flex items-center gap-2">
                  <span className="inline-flex items-center gap-1 px-2.5 py-1 rounded-full text-xs font-semibold bg-[#e8f4f0] text-[#1e6152]">
                    <Award size={13} className="text-[#2c8069]" />
                    {doctor.specialty}
                  </span>
                  {doctor.experienceYears ? (
                    <span className="text-[11px] text-[#869692] font-medium">
                      {doctor.experienceYears}+ years exp
                    </span>
                  ) : null}
                </div>

                {/* Key Doctor Details: Department, Room Number */}
                <div className="mt-4 space-y-2 text-xs text-[#526a64]">
                  <div className="flex items-center gap-2">
                    <Building2 size={14} className="text-[#849a94] shrink-0" />
                    <span className="text-[#849a94]">Department:</span>
                    <strong className="text-[#143d38] font-semibold">
                      {doctor.department}
                    </strong>
                  </div>

                  <div className="flex items-center gap-2">
                    <MapPin size={14} className="text-[#849a94] shrink-0" />
                    <span className="text-[#849a94]">Room Number:</span>
                    <strong className="text-[#143d38] font-semibold">
                      {doctor.roomNumber}
                    </strong>
                  </div>

                  {doctor.phone && (
                    <div className="flex items-center gap-2 text-[#738480]">
                      <Phone size={13} className="text-[#849a94] shrink-0" />
                      <span>{doctor.phone}</span>
                    </div>
                  )}

                  {doctor.email && (
                    <div className="flex items-center gap-2 text-[#738480] truncate">
                      <Mail size={13} className="text-[#849a94] shrink-0" />
                      <span className="truncate">{doctor.email}</span>
                    </div>
                  )}
                </div>

                {/* Consultation Fee (in PKR) Card Section */}
                <div className="mt-4 p-2.5 rounded-lg bg-[#f6faf8] border border-[#e2ede8] flex items-center justify-between">
                  <span className="text-xs font-medium text-[#657d77] flex items-center gap-1">
                    <DollarSign size={14} className="text-[#3e8c79]" />
                    Consultation Fee
                  </span>
                  <span className="text-sm font-extrabold text-[#176153]">
                    PKR {doctor.consultationFee.toLocaleString()}
                  </span>
                </div>
              </div>

              {/* Functional Edit and Delete Buttons */}
              <div className="mt-5 pt-3.5 border-t border-[#edf2f0] flex items-center justify-end gap-2">
                <button
                  type="button"
                  onClick={() => handleOpenEdit(doctor)}
                  className="inline-flex items-center gap-1.5 px-3 py-1.5 text-xs font-semibold rounded-lg border border-[#d6e4df] text-[#286354] hover:bg-[#eef8f4] hover:border-[#b3d4cb] transition"
                  aria-label={`Edit ${doctor.name}`}
                >
                  <Pencil size={13} />
                  Edit
                </button>
                <button
                  type="button"
                  onClick={() => handleDeleteClick(doctor)}
                  className="inline-flex items-center gap-1.5 px-3 py-1.5 text-xs font-semibold rounded-lg border border-[#f5d7d7] text-[#c04b4b] hover:bg-[#fdf2f2] hover:border-[#e9b5b5] transition"
                  aria-label={`Delete ${doctor.name}`}
                >
                  <Trash2 size={13} />
                  Delete
                </button>
              </div>
            </article>
          ))}
        </div>
      ) : (
        /* Empty State */
        <div className="bg-white rounded-xl border border-[#e0ebe8] p-12 text-center shadow-sm">
          <div className="w-14 h-14 rounded-full bg-[#f0f7f4] text-[#3e8c79] grid place-items-center mx-auto mb-3">
            <Stethoscope size={28} />
          </div>
          <h3 className="text-base font-bold text-[#143d38]">No doctors found</h3>
          <p className="mt-1 text-xs text-[#738480] max-w-sm mx-auto">
            No physicians matched your current filter or search criteria. Try modifying your
            specialty, department, or availability selections.
          </p>
          <button
            onClick={resetFilters}
            className="mt-4 inline-flex items-center gap-1.5 rounded-lg bg-[#176153] px-4 py-2 text-xs font-semibold text-white hover:bg-[#124e44] transition"
          >
            <RotateCcw size={13} />
            Reset All Filters
          </button>
        </div>
      )}

      {/* Add / Edit Doctor Modal */}
      {isModalOpen && (
        <div
          className="fixed inset-0 z-50 bg-[#153d38]/40 backdrop-blur-xs grid place-items-center p-4 overflow-y-auto"
          onClick={() => setIsModalOpen(false)}
        >
          <div
            role="dialog"
            aria-modal="true"
            aria-labelledby="doctor-modal-title"
            onClick={(e) => e.stopPropagation()}
            className="w-full max-w-lg bg-white rounded-2xl shadow-2xl border border-[#dcebe5] overflow-hidden my-8"
          >
            {/* Modal Header */}
            <div className="flex items-center justify-between px-6 py-4 border-b border-[#e5e7eb] bg-[#fafcfb]">
              <div>
                <p className="text-[11px] font-bold uppercase tracking-wider text-[#3e8c79]">
                  {editingDoctor ? 'Edit Doctor Profile' : 'New Practitioner'}
                </p>
                <h2 id="doctor-modal-title" className="text-lg font-extrabold text-[#143d38]">
                  {editingDoctor ? `Edit ${editingDoctor.name}` : 'Add New Doctor'}
                </h2>
              </div>
              <button
                type="button"
                onClick={() => setIsModalOpen(false)}
                className="w-8 h-8 rounded-lg grid place-items-center text-[#82938f] hover:bg-[#eef7f3] hover:text-[#143d38] transition"
                aria-label="Close modal"
              >
                <X size={18} />
              </button>
            </div>

            {/* Modal Form */}
            <form onSubmit={handleFormSubmit} className="p-6 space-y-4">
              <div>
                <label className="block text-xs font-bold text-[#3d5a54] mb-1">
                  Full Name *
                </label>
                <input
                  type="text"
                  required
                  placeholder="e.g. Dr. Ayesha Khan"
                  value={formData.name}
                  onChange={(e) => setFormData({ ...formData, name: e.target.value })}
                  className="w-full h-10 rounded-lg border border-[#dce6e3] px-3 text-xs text-[#1f2937] outline-none focus:border-[#4f9a87] focus:ring-2 focus:ring-[#4f9a87]/20"
                />
                {formErrors.name && (
                  <p className="text-[11px] text-[#dc2626] mt-1">{formErrors.name}</p>
                )}
              </div>

              <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
                <div>
                  <label className="block text-xs font-bold text-[#3d5a54] mb-1">
                    Specialty *
                  </label>
                  <select
                    value={formData.specialty}
                    onChange={(e) => setFormData({ ...formData, specialty: e.target.value })}
                    className="w-full h-10 rounded-lg border border-[#dce6e3] bg-white px-3 text-xs text-[#1f2937] outline-none focus:border-[#4f9a87] focus:ring-2 focus:ring-[#4f9a87]/20 cursor-pointer"
                  >
                    <option value="Pediatricians">Pediatricians</option>
                    <option value="Cardiologist">Cardiologist</option>
                    <option value="Neurologist">Neurologist</option>
                    <option value="Dermatologist">Dermatologist</option>
                    <option value="Orthopedic Surgeon">Orthopedic Surgeon</option>
                    <option value="General Physician">General Physician</option>
                  </select>
                </div>

                <div>
                  <label className="block text-xs font-bold text-[#3d5a54] mb-1">
                    Department *
                  </label>
                  <select
                    value={formData.department}
                    onChange={(e) => setFormData({ ...formData, department: e.target.value })}
                    className="w-full h-10 rounded-lg border border-[#dce6e3] bg-white px-3 text-xs text-[#1f2937] outline-none focus:border-[#4f9a87] focus:ring-2 focus:ring-[#4f9a87]/20 cursor-pointer"
                  >
                    <option value="Pediatrics">Pediatrics</option>
                    <option value="Cardiology">Cardiology</option>
                    <option value="Neurology">Neurology</option>
                    <option value="Dermatology">Dermatology</option>
                    <option value="Orthopedics">Orthopedics</option>
                    <option value="General Medicine">General Medicine</option>
                  </select>
                </div>
              </div>

              <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
                <div>
                  <label className="block text-xs font-bold text-[#3d5a54] mb-1">
                    Room Number *
                  </label>
                  <input
                    type="text"
                    required
                    placeholder="e.g. Room 204 - OPD Block A"
                    value={formData.roomNumber}
                    onChange={(e) =>
                      setFormData({ ...formData, roomNumber: e.target.value })
                    }
                    className="w-full h-10 rounded-lg border border-[#dce6e3] px-3 text-xs text-[#1f2937] outline-none focus:border-[#4f9a87] focus:ring-2 focus:ring-[#4f9a87]/20"
                  />
                  {formErrors.roomNumber && (
                    <p className="text-[11px] text-[#dc2626] mt-1">
                      {formErrors.roomNumber}
                    </p>
                  )}
                </div>

                <div>
                  <label className="block text-xs font-bold text-[#3d5a54] mb-1">
                    Consultation Fee (in PKR) *
                  </label>
                  <input
                    type="number"
                    min="0"
                    step="100"
                    required
                    placeholder="e.g. 3500"
                    value={formData.consultationFee}
                    onChange={(e) =>
                      setFormData({
                        ...formData,
                        consultationFee: Number(e.target.value),
                      })
                    }
                    className="w-full h-10 rounded-lg border border-[#dce6e3] px-3 text-xs text-[#1f2937] outline-none focus:border-[#4f9a87] focus:ring-2 focus:ring-[#4f9a87]/20"
                  />
                  {formErrors.consultationFee && (
                    <p className="text-[11px] text-[#dc2626] mt-1">
                      {formErrors.consultationFee}
                    </p>
                  )}
                </div>
              </div>

              <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
                <div>
                  <label className="block text-xs font-bold text-[#3d5a54] mb-1">
                    Availability Status *
                  </label>
                  <select
                    value={formData.availability}
                    onChange={(e) =>
                      setFormData({
                        ...formData,
                        availability: e.target.value as DoctorAvailability,
                      })
                    }
                    className="w-full h-10 rounded-lg border border-[#dce6e3] bg-white px-3 text-xs text-[#1f2937] outline-none focus:border-[#4f9a87] focus:ring-2 focus:ring-[#4f9a87]/20 cursor-pointer"
                  >
                    <option value="Available">Available</option>
                    <option value="On Leave">On Leave</option>
                    <option value="In Surgery">In Surgery</option>
                  </select>
                </div>

                <div>
                  <label className="block text-xs font-bold text-[#3d5a54] mb-1">
                    Experience (Years)
                  </label>
                  <input
                    type="number"
                    min="0"
                    max="60"
                    placeholder="e.g. 10"
                    value={formData.experienceYears}
                    onChange={(e) =>
                      setFormData({
                        ...formData,
                        experienceYears: Number(e.target.value),
                      })
                    }
                    className="w-full h-10 rounded-lg border border-[#dce6e3] px-3 text-xs text-[#1f2937] outline-none focus:border-[#4f9a87] focus:ring-2 focus:ring-[#4f9a87]/20"
                  />
                </div>
              </div>

              <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
                <div>
                  <label className="block text-xs font-bold text-[#3d5a54] mb-1">
                    Phone Number
                  </label>
                  <input
                    type="text"
                    placeholder="0300-1234567"
                    value={formData.phone}
                    onChange={(e) => setFormData({ ...formData, phone: e.target.value })}
                    className="w-full h-10 rounded-lg border border-[#dce6e3] px-3 text-xs text-[#1f2937] outline-none focus:border-[#4f9a87] focus:ring-2 focus:ring-[#4f9a87]/20"
                  />
                </div>

                <div>
                  <label className="block text-xs font-bold text-[#3d5a54] mb-1">
                    Email Address
                  </label>
                  <input
                    type="email"
                    placeholder="doctor@carepoint.pk"
                    value={formData.email}
                    onChange={(e) => setFormData({ ...formData, email: e.target.value })}
                    className="w-full h-10 rounded-lg border border-[#dce6e3] px-3 text-xs text-[#1f2937] outline-none focus:border-[#4f9a87] focus:ring-2 focus:ring-[#4f9a87]/20"
                  />
                </div>
              </div>

              <div>
                <label className="block text-xs font-bold text-[#3d5a54] mb-1">
                  Qualifications & Credentials
                </label>
                <input
                  type="text"
                  placeholder="e.g. MBBS, FCPS, MRCP"
                  value={formData.qualification}
                  onChange={(e) =>
                    setFormData({ ...formData, qualification: e.target.value })
                  }
                  className="w-full h-10 rounded-lg border border-[#dce6e3] px-3 text-xs text-[#1f2937] outline-none focus:border-[#4f9a87] focus:ring-2 focus:ring-[#4f9a87]/20"
                />
              </div>

              {/* Form Action Buttons */}
              <div className="pt-4 border-t border-[#e5e7eb] flex items-center justify-end gap-3">
                <button
                  type="button"
                  onClick={() => setIsModalOpen(false)}
                  className="px-4 py-2 text-xs font-semibold rounded-lg border border-[#d6e4df] text-[#55726b] hover:bg-[#f3f7f5] transition"
                >
                  Cancel
                </button>
                <button
                  type="submit"
                  className="px-5 py-2 text-xs font-semibold rounded-lg bg-[#176153] hover:bg-[#124e44] text-white shadow-sm transition"
                >
                  {editingDoctor ? 'Save Changes' : 'Add Doctor'}
                </button>
              </div>
            </form>
          </div>
        </div>
      )}

      {/* Delete Confirmation Modal */}
      {deleteTarget && (
        <div
          className="fixed inset-0 z-50 bg-[#153d38]/40 backdrop-blur-xs grid place-items-center p-4"
          onClick={() => setDeleteTarget(null)}
        >
          <div
            role="dialog"
            aria-modal="true"
            aria-labelledby="delete-dialog-title"
            onClick={(e) => e.stopPropagation()}
            className="w-full max-w-md bg-white rounded-2xl shadow-2xl border border-[#f3d3d3] p-6 text-center"
          >
            <div className="w-12 h-12 rounded-full bg-[#fde8e8] text-[#dc2626] grid place-items-center mx-auto mb-3">
              <AlertTriangle size={24} />
            </div>
            <h3 id="delete-dialog-title" className="text-base font-bold text-[#143d38]">
              Delete Doctor Record?
            </h3>
            <p className="mt-2 text-xs text-[#738480] leading-relaxed">
              Are you sure you want to remove <strong>{deleteTarget.name}</strong> ({deleteTarget.specialty}) from the directory?
            </p>
            <div className="mt-6 flex items-center justify-center gap-3">
              <button
                type="button"
                onClick={() => setDeleteTarget(null)}
                className="px-4 py-2 text-xs font-semibold rounded-lg border border-[#d6e4df] text-[#55726b] hover:bg-[#f3f7f5] transition"
              >
                Cancel
              </button>
              <button
                type="button"
                onClick={handleConfirmDelete}
                className="px-4 py-2 text-xs font-semibold rounded-lg bg-[#dc2626] hover:bg-[#b91c1c] text-white shadow-sm transition"
              >
                Yes, Delete Doctor
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
