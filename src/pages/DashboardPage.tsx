
import {
  Users,
  Stethoscope,
  CalendarDays,
  Siren,
  Clock,
  ArrowUpRight,
  ArrowRight,
  CheckCircle2,
  AlertTriangle,
} from 'lucide-react';

import StatCard from '../components/StatCard';
import {
  dashboardAppointments as appointments,
  dashboardEmergencies,
} from '../mockdata/dashboard';

export default function DashboardPage() {
  return (
    <div className="dashboard">
      <section className="dashboard-welcome">
        <div>
          <p className="dashboard-eyebrow">HOSPITAL OVERVIEW</p>
          <h2>Good morning, Administrator</h2>
          <p>Here is what's happening at your hospital today.</p>
        </div>

        <div className="dashboard-date">
          <CalendarDays size={18} />
          <span>Today's Overview</span>
        </div>
      </section>

      <section
        className="dashboard-stats"
        aria-label="Hospital statistics"
      >
        <StatCard
          title="Total Patients"
          value="1,248"
          icon={Users}
          color="green"
          subtitle="Registered patients"
        />

        <StatCard
          title="Available Doctors"
          value="32"
          icon={Stethoscope}
          color="blue"
          subtitle="Across all departments"
        />

        <StatCard
          title="Today's Appointments"
          value="86"
          icon={CalendarDays}
          color="amber"
          subtitle="12 appointments remaining"
        />

        <StatCard
          title="Emergency Cases"
          value="5"
          icon={Siren}
          color="red"
          subtitle="Requires attention"
        />
      </section>

      <section className="dashboard-main-grid">
        <div className="dashboard-panel appointments-panel">
          <div className="panel-heading">
            <div>
              <h3>Recent Appointments</h3>
              <p>Overview of scheduled patient visits</p>
            </div>

            <a href="/appointments" className="panel-link">
              View all <ArrowRight size={16} />
            </a>
          </div>

          <div className="appointments-table-wrapper">
            <table className="appointments-table">
              <thead>
                <tr>
                  <th>Patient</th>
                  <th>Doctor / Department</th>
                  <th>Time</th>
                  <th>Status</th>
                </tr>
              </thead>

              <tbody>
                {appointments.map((appointment) => (
                  <tr key={appointment.id}>
                    <td>
                      <div className="patient-cell">
                        <span className="patient-avatar">
                          {appointment.patient.charAt(0)}
                        </span>

                        <div>
                          <strong>{appointment.patient}</strong>
                          <small>{appointment.id}</small>
                        </div>
                      </div>
                    </td>

                    <td>
                      <strong className="doctor-name">
                        {appointment.doctor}
                      </strong>

                      <small className="department-name">
                        {appointment.department}
                      </small>
                    </td>

                    <td>
                      <span className="appointment-time">
                        <Clock size={14} />
                        {appointment.time}
                      </span>
                    </td>

                    <td>
                      <span
                        className={`appointment-status status-${appointment.status.toLowerCase()}`}
                      >
                        {appointment.status}
                      </span>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </div>

        <div className="dashboard-panel emergency-panel">
          <div className="panel-heading">
            <div>
              <h3>Emergency Alerts</h3>
              <p>Items needing attention</p>
            </div>

            <span className="emergency-count">
              {dashboardEmergencies.length}
            </span>
          </div>

          <div className="emergency-list">
            {dashboardEmergencies.map((emergency) => (
              <article
                className="emergency-item"
                key={emergency.id}
              >
                <div className="emergency-icon">
                  <AlertTriangle size={19} />
                </div>

                <div className="emergency-details">
                  <div className="emergency-title-row">
                    <strong>{emergency.title}</strong>

                    <span
                      className={`severity severity-${emergency.severity.toLowerCase()}`}
                    >
                      {emergency.severity}
                    </span>
                  </div>

                  <p>{emergency.description}</p>
                </div>
              </article>
            ))}
          </div>

          <a href="/emergency" className="emergency-footer-link">
            View emergency centre <ArrowUpRight size={16} />
          </a>
        </div>
      </section>

      <section className="dashboard-bottom-grid">
        <div className="dashboard-panel quick-summary">
          <div className="panel-heading">
            <div>
              <h3>Daily Summary</h3>
              <p>Hospital activity at a glance</p>
            </div>
          </div>

          <div className="summary-row">
            <span className="summary-icon summary-green">
              <CheckCircle2 size={18} />
            </span>
            <span>Appointments completed</span>
            <strong>24</strong>
          </div>

          <div className="summary-row">
            <span className="summary-icon summary-blue">
              <Users size={18} />
            </span>
            <span>Patients checked in</span>
            <strong>42</strong>
          </div>

          <div className="summary-row">
            <span className="summary-icon summary-amber">
              <Clock size={18} />
            </span>
            <span>Patients waiting</span>
            <strong>8</strong>
          </div>
        </div>

        <div className="dashboard-panel system-status">
          <div className="panel-heading">
            <div>
              <h3>System Status</h3>
              <p>Current operational overview</p>
            </div>
          </div>

          <div className="system-status-indicator">
            <span className="system-status-dot" />
            <div>
              <strong>All core modules available</strong>
              <p>Demonstration data is being displayed.</p>
            </div>
          </div>

          <div className="system-status-note">
            This dashboard currently uses sample data. Live backend
            integration will be added later.
          </div>
        </div>
      </section>
    </div>
  );
}
