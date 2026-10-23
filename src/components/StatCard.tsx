
import type { LucideIcon } from 'lucide-react';

interface StatCardProps {
  title: string;
  value: string | number;
  icon: LucideIcon;
  color?: 'green' | 'blue' | 'amber' | 'red';
  subtitle?: string;
}

export default function StatCard({
  title,
  value,
  icon: Icon,
  color = 'green',
  subtitle,
}: StatCardProps) {
  return (
    <div className={`stat-card stat-card-${color}`}>
      <div className="stat-card-content">
        <p className="stat-card-title">{title}</p>
        <h2 className="stat-card-value">{value}</h2>
        {subtitle && (
          <p className="stat-card-subtitle">{subtitle}</p>
        )}
      </div>

      <div className="stat-card-icon">
        <Icon size={24} />
      </div>
    </div>
  );
}
