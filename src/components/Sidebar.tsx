
import { NavLink } from 'react-router-dom';
import {
  LayoutDashboard,
  Users,
  Stethoscope,
  CalendarDays,
  Siren,
  Navigation,
  Receipt,
  Hospital,
  Settings,
} from 'lucide-react';

const navigationItems = [
  { label: 'Dashboard', path: '/', icon: LayoutDashboard },
  { label: 'Patients', path: '/patients', icon: Users },
  { label: 'Doctors & Departments', path: '/doctors', icon: Stethoscope },
  { label: 'Appointments', path: '/appointments', icon: CalendarDays },
  { label: 'Emergency', path: '/emergency', icon: Siren },
  { label: 'Navigation', path: '/navigation', icon: Navigation },
  { label: 'Billing', path: '/billing', icon: Receipt },
];

export default function Sidebar() {
  return (
    <aside className="app-sidebar h-full min-h-screen">
      <div className="sidebar-brand">
        <div className="sidebar-logo">
          <Hospital size={25} />
        </div>

        <div className="sidebar-brand-text">
          <h2>CarePoint</h2>
          <span>Hospital Management</span>
        </div>
      </div>

      <div className="sidebar-section-label">MAIN MENU</div>

      <nav className="sidebar-navigation" aria-label="Main navigation">
        {navigationItems.map(({ label, path, icon: Icon }) => (
          <NavLink
            key={path}
            to={path}
            end={path === '/'}
            className={({ isActive }) =>
              `sidebar-link${isActive ? ' active' : ''}`
            }
          >
            <Icon size={19} />
            <span>{label}</span>
          </NavLink>
        ))}
      </nav>

      <div className="sidebar-footer">
        <NavLink
          to="/settings"
          className={({ isActive }) =>
            `sidebar-link${isActive ? ' active' : ''}`
          }
        >
          <Settings size={19} />
          <span>Settings</span>
        </NavLink>

        <div className="sidebar-version">Hospital Management System · v1.0</div>
      </div>
    </aside>
  );
}
