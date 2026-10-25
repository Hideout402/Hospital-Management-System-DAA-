
import { Outlet, useLocation } from 'react-router-dom';
import { useState } from 'react';
import Sidebar from '../components/Sidebar';
import Header from '../components/Header';

const pageDetails: Record<string, { title: string; subtitle: string }> = {
  '/': {
    title: 'Dashboard',
    subtitle: 'Overview of hospital operations',
  },
  '/patients': {
    title: 'Patients',
    subtitle: 'Manage patient records',
  },
  '/doctors': {
    title: 'Doctors & Departments',
    subtitle: 'Manage doctors and departments',
  },
  '/appointments': {
    title: 'Appointments',
    subtitle: 'Manage hospital appointments',
  },
  '/emergency': {
    title: 'Emergency',
    subtitle: 'Monitor emergency cases',
  },
  '/billing': {
    title: 'Billing',
    subtitle: 'Manage invoices and payments',
  },
  '/settings': {
    title: 'Settings',
    subtitle: 'Application preferences',
  },
};

export default function MainLayout() {
  const location = useLocation();
  const [mobileMenuOpen, setMobileMenuOpen] = useState(false);

  const details = pageDetails[location.pathname] ?? {
    title: 'Hospital Management System',
    subtitle: 'Hospital operations',
  };

  return (
    <div className="app-layout">
      <div
        className={`sidebar-wrapper sticky top-0 h-screen shrink-0 self-start z-40 overflow-y-auto ${
          mobileMenuOpen ? 'mobile-open' : ''
        }`}
      >
        <Sidebar />
      </div>

      <div className="app-main">
        <button
          type="button"
          className="mobile-menu-button"
          onClick={() => setMobileMenuOpen((open) => !open)}
          aria-label="Toggle navigation"
        >
          Menu
        </button>

        <Header title={details.title} subtitle={details.subtitle} />

        <main className="page-content" onClick={() => setMobileMenuOpen(false)}>
          <Outlet />
        </main>
      </div>
    </div>
  );
}
