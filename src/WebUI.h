#pragma once
#include <Arduino.h>

// =============================================================================
// EMBEDDED WEB INTERFACE (RESPONSIVE, ZERO-DEPENDENCY, DARK MODE)
// Stored in PROGMEM flash for instant <50ms loading on any smartphone/laptop.
// =============================================================================

const char INDEX_HTML[] PROGMEM = R"rawhtml(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
<title>XiaoVault Survival Wiki</title>
<style>
:root {
  --bg: #0d1117;
  --card: #161b22;
  --border: #30363d;
  --text: #c9d1d9;
  --text-bright: #f0f6fc;
  --accent: #f0883e;
  --accent-hover: #fa9a52;
  --danger: #f85149;
  --success: #3fb950;
  --font: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
}
* { box-sizing: border-box; margin: 0; padding: 0; }
body {
  background: var(--bg);
  color: var(--text);
  font-family: var(--font);
  line-height: 1.6;
  padding-bottom: 60px;
}
header {
  background: var(--card);
  border-bottom: 1px solid var(--border);
  padding: 12px 16px;
  position: sticky;
  top: 0;
  z-index: 100;
  display: flex;
  align-items: center;
  justify-content: space-between;
}
.brand {
  display: flex;
  align-items: center;
  gap: 10px;
  font-weight: 700;
  color: var(--text-bright);
  font-size: 1.05rem;
  text-decoration: none;
}
.brand .badge {
  background: var(--accent);
  color: #000;
  font-size: 0.65rem;
  font-weight: 800;
  padding: 2px 6px;
  border-radius: 4px;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}
.status-pill {
  font-size: 0.75rem;
  color: #8b949e;
  display: flex;
  align-items: center;
  gap: 6px;
}
.dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: var(--success);
  display: inline-block;
}
.container {
  max-width: 720px;
  margin: 0 auto;
  padding: 16px;
}
.search-box {
  margin: 16px 0 20px 0;
  position: relative;
}
.search-input {
  width: 100%;
  padding: 14px 16px 14px 44px;
  background: var(--card);
  border: 1px solid var(--border);
  border-radius: 8px;
  color: var(--text-bright);
  font-size: 1rem;
  outline: none;
  transition: border-color 0.2s;
}
.search-input:focus {
  border-color: var(--accent);
}
.search-icon {
  position: absolute;
  left: 14px;
  top: 50%;
  transform: translateY(-50%);
  color: #8b949e;
}
.categories {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 10px;
  margin-bottom: 24px;
}
@media (max-width: 480px) {
  .categories { grid-template-columns: repeat(2, 1fr); }
}
.cat-btn {
  background: var(--card);
  border: 1px solid var(--border);
  border-radius: 8px;
  padding: 12px;
  text-align: center;
  color: var(--text-bright);
  text-decoration: none;
  font-size: 0.85rem;
  font-weight: 600;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 6px;
  transition: all 0.15s ease;
  cursor: pointer;
}
.cat-btn:hover, .cat-btn:active {
  border-color: var(--accent);
  background: #1c2128;
}
.cat-icon { font-size: 1.4rem; }
.card {
  background: var(--card);
  border: 1px solid var(--border);
  border-radius: 8px;
  padding: 16px;
  margin-bottom: 16px;
}
.card h2 {
  color: var(--text-bright);
  font-size: 1.15rem;
  margin-bottom: 12px;
  display: flex;
  align-items: center;
  gap: 8px;
}
.guide-item {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 10px 0;
  border-bottom: 1px solid #21262d;
  color: var(--text);
  text-decoration: none;
  font-size: 0.95rem;
}
.guide-item:last-child { border-bottom: none; }
.guide-item:hover { color: var(--accent); }
.article-view {
  display: none;
  background: var(--card);
  border: 1px solid var(--border);
  border-radius: 8px;
  padding: 20px;
}
.article-header {
  border-bottom: 1px solid var(--border);
  padding-bottom: 12px;
  margin-bottom: 16px;
  display: flex;
  align-items: center;
  justify-content: space-between;
}
.article-view h3 { color: var(--accent); margin-bottom: 10px; font-size: 1.25rem; }
.article-view h4 { color: var(--text-bright); margin: 14px 0 8px 0; font-size: 1.05rem; }
.article-view p { margin-bottom: 12px; }
.article-view ul, .article-view ol { margin-left: 24px; margin-bottom: 12px; }
.article-view code, .article-view pre {
  background: #0d1117;
  border: 1px solid var(--border);
  padding: 2px 6px;
  border-radius: 4px;
  font-family: monospace;
}
.article-view pre { padding: 12px; overflow-x: auto; margin-bottom: 12px; }
.btn {
  background: var(--border);
  color: var(--text-bright);
  border: none;
  padding: 8px 14px;
  border-radius: 6px;
  cursor: pointer;
  font-size: 0.85rem;
  font-weight: 600;
}
.btn-accent { background: var(--accent); color: #000; }
.sys-info {
  display: flex;
  justify-content: space-around;
  text-align: center;
  font-size: 0.8rem;
  color: #8b949e;
  margin-top: 24px;
  padding-top: 16px;
  border-top: 1px solid var(--border);
}
.sys-val { font-weight: 700; color: var(--text-bright); font-size: 0.95rem; }
</style>
</head>
<body>

<header>
  <a href="/" class="brand" onclick="showHome(event)">
    <span>⚡ XiaoVault</span>
    <span class="badge">OFFLINE</span>
  </a>
  <div class="status-pill">
    <span class="dot" id="sdDot"></span>
    <span id="sdStatus">SD Online</span>
  </div>
</header>

<div class="container">
  <div id="homeView">
    <div class="search-box">
      <span class="search-icon">🔍</span>
      <input type="text" id="searchInput" class="search-input" placeholder="Search offline Wikipedia & guides..." autocomplete="off">
    </div>

    <div class="categories">
      <div class="cat-btn" onclick="filterCategory('First Aid')">
        <span class="cat-icon">🩺</span>
        <span>First Aid</span>
      </div>
      <div class="cat-btn" onclick="filterCategory('Hydration')">
        <span class="cat-icon">💧</span>
        <span>Water</span>
      </div>
      <div class="cat-btn" onclick="filterCategory('Shelter')">
        <span class="cat-icon">🏕️</span>
        <span>Shelter</span>
      </div>
      <div class="cat-btn" onclick="filterCategory('Signaling')">
        <span class="cat-icon">📡</span>
        <span>Morse & Radio</span>
      </div>
      <div class="cat-btn" onclick="filterCategory('Foraging')">
        <span class="cat-icon">🌿</span>
        <span>Edibles</span>
      </div>
      <div class="cat-btn" onclick="browseSD()">
        <span class="cat-icon">📁</span>
        <span>Browse SD</span>
      </div>
    </div>

    <div class="card" id="resultsCard">
      <h2 id="cardTitle">🚨 Emergency Fast Triage (Flash Memory)</h2>
      <div id="guidesList">
        <!-- Rendered dynamically -->
      </div>
    </div>

    <div class="sys-info">
      <div>
        <div class="sys-val" id="battVal">3.95 V</div>
        <div>Battery</div>
      </div>
      <div>
        <div class="sys-val" id="storageVal">-- / --</div>
        <div>MicroSD</div>
      </div>
      <div>
        <div class="sys-val" id="clientsVal">1</div>
        <div>Readers</div>
      </div>
      <div>
        <div class="sys-val" id="uptimeVal">--</div>
        <div>Uptime</div>
      </div>
    </div>
  </div>

  <div id="articleView" class="article-view">
    <div class="article-header">
      <button class="btn" onclick="closeArticle()">← Back</button>
      <button class="btn btn-accent" id="bookmarkBtn" onclick="toggleBookmark()">⭐ Bookmark</button>
    </div>
    <div id="articleContent"></div>
  </div>
</div>

<script>
let currentArticleId = "";
let cachedGuides = [];

async function loadInitialData() {
  try {
    const res = await fetch('/api/emergency');
    cachedGuides = await res.json();
    renderGuides(cachedGuides);
  } catch (e) {
    console.log("Using static emergency cache");
  }
  updateTelemetry();
  setInterval(updateTelemetry, 10000);
}

function renderGuides(guides) {
  const container = document.getElementById('guidesList');
  if (!guides || guides.length === 0) {
    container.innerHTML = '<p style="color:#8b949e; padding:10px 0;">No matching articles found.</p>';
    return;
  }
  container.innerHTML = guides.map(g => `
    <a href="#" class="guide-item" onclick="openGuide('${g.id}', event)">
      <span>${g.icon || '📄'} ${g.title}</span>
      <span style="font-size:0.75rem; color:#8b949e;">${g.category || 'Guide'} →</span>
    </a>
  `).join('');
}

document.getElementById('searchInput').addEventListener('input', async (e) => {
  const q = e.target.value.trim().toLowerCase();
  if (!q) {
    document.getElementById('cardTitle').innerText = "🚨 Emergency Fast Triage (Flash Memory)";
    renderGuides(cachedGuides);
    return;
  }
  document.getElementById('cardTitle').innerText = `Search Results: "${q}"`;
  
  // Try querying server SD index
  try {
    const res = await fetch(`/api/search?q=${encodeURIComponent(q)}`);
    const serverResults = await res.json();
    if (serverResults && serverResults.length > 0) {
      renderGuides(serverResults);
      return;
    }
  } catch(err) {}

  // Fallback to client filter of emergency guides
  const localMatch = cachedGuides.filter(g => 
    g.title.toLowerCase().includes(q) || (g.category && g.category.toLowerCase().includes(q))
  );
  renderGuides(localMatch);
});

async function openGuide(id, e) {
  if (e) e.preventDefault();
  currentArticleId = id;
  const articleView = document.getElementById('articleView');
  const homeView = document.getElementById('homeView');
  const content = document.getElementById('articleContent');
  
  // Check if emergency flash guide
  const flashGuide = cachedGuides.find(g => g.id === id);
  if (flashGuide && flashGuide.contentHtml) {
    content.innerHTML = flashGuide.contentHtml;
    homeView.style.display = 'none';
    articleView.style.display = 'block';
    window.scrollTo(0,0);
    return;
  }

  // Otherwise fetch from SD wiki engine
  content.innerHTML = '<p>Loading article from MicroSD...</p>';
  homeView.style.display = 'none';
  articleView.style.display = 'block';
  window.scrollTo(0,0);

  try {
    const res = await fetch(`/wiki?id=${encodeURIComponent(id)}`);
    content.innerHTML = await res.text();
  } catch (err) {
    content.innerHTML = '<p style="color:var(--danger)">Error loading article from MicroSD card.</p>';
  }
}

function closeArticle() {
  document.getElementById('articleView').style.display = 'none';
  document.getElementById('homeView').style.display = 'block';
}

function showHome(e) {
  if (e) e.preventDefault();
  closeArticle();
  document.getElementById('searchInput').value = "";
  renderGuides(cachedGuides);
}

function filterCategory(cat) {
  document.getElementById('cardTitle').innerText = `Category: ${cat}`;
  const filtered = cachedGuides.filter(g => g.category === cat);
  renderGuides(filtered);
}

async function updateTelemetry() {
  try {
    const res = await fetch('/api/status');
    const data = await res.json();
    if (data.sd_mounted) {
      document.getElementById('sdDot').style.background = 'var(--success)';
      document.getElementById('sdStatus').innerText = 'SD Online';
      document.getElementById('storageVal').innerText = `${data.sd_used_mb}MB / ${data.sd_total_mb}MB`;
    } else {
      document.getElementById('sdDot').style.background = 'var(--danger)';
      document.getElementById('sdStatus').innerText = 'No SD Card';
      document.getElementById('storageVal').innerText = 'Flash Only';
    }
    document.getElementById('battVal').innerText = data.battery_v > 0 ? `${data.battery_v.toFixed(2)} V` : 'USB 5V';
    document.getElementById('clientsVal').innerText = data.wifi_clients;
    document.getElementById('uptimeVal').innerText = `${Math.floor(data.uptime_sec / 60)}m`;
  } catch(e) {}
}

function toggleBookmark() {
  let bookmarks = JSON.parse(localStorage.getItem('xv_bookmarks') || '[]');
  const idx = bookmarks.indexOf(currentArticleId);
  if (idx >= 0) {
    bookmarks.splice(idx, 1);
    alert('Bookmark removed.');
  } else {
    bookmarks.push(currentArticleId);
    alert('Article bookmarked in phone storage!');
  }
  localStorage.setItem('xv_bookmarks', JSON.stringify(bookmarks));
}

function browseSD() {
  window.location.href = '/browse';
}

loadInitialData();
</script>
</body>
</html>
)rawhtml";
