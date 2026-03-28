// Liminal Spaces - Landing Page Scripts

// Smooth reveal on scroll
const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            entry.target.classList.add('visible');
        }
    });
}, { threshold: 0.1 });

document.querySelectorAll('.feature-card, .map-card, .platform-card, .stat').forEach(el => {
    el.style.opacity = '0';
    el.style.transform = 'translateY(20px)';
    el.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
    observer.observe(el);
});

document.addEventListener('DOMContentLoaded', () => {
    // Add visible class handler
    const style = document.createElement('style');
    style.textContent = '.visible { opacity: 1 !important; transform: translateY(0) !important; }';
    document.head.appendChild(style);

    // Stagger animations for grid items
    document.querySelectorAll('.features-grid, .maps-grid, .platforms-grid, .about-stats').forEach(grid => {
        const items = grid.children;
        Array.from(items).forEach((item, i) => {
            item.style.transitionDelay = `${i * 0.1}s`;
        });
    });
});

// Nav background on scroll
const nav = document.querySelector('.nav');
window.addEventListener('scroll', () => {
    if (window.scrollY > 50) {
        nav.style.borderBottomColor = '#333';
    } else {
        nav.style.borderBottomColor = '#222';
    }
});
