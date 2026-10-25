
import { Bell } from 'lucide-react';

interface HeaderProps {
  title: string;
  subtitle?: string;
}

export default function Header({ title, subtitle }: HeaderProps) {
  return (
    <header className="app-header">
      <div className="header-heading">
        <h1>{title}</h1>
        {subtitle && <p>{subtitle}</p>}
      </div>

      <div className="header-actions">

        <button
          type="button"
          className="header-notification"
          aria-label="Notifications"
        >
          <Bell size={20} />
          <span className="notification-dot" />
        </button>

        <div className="header-user">
          <div className="header-avatar">A</div>
          <div className="header-user-info">
            <span className="header-user-name">Administrator</span>
            <span className="header-user-role">Hospital Staff</span>
          </div>
        </div>
      </div>
    </header>
  );
}
