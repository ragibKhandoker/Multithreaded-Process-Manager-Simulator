// Step-by-step replay of: ./pm_sim thread1.txt thread2.txt
const R = (pid, ppid) => ({ pid, ppid, state: "RUNNING", exit: "-" });

const steps = [
  { t: 0, msg: "Start: only the init process (PID 1) exists. Monitor thread writes the first snapshot.",
    table: [R(1, 0)] },
  { t: 0, msg: "Thread 1 → fork 1 creates PID 2. Thread 1 then sleeps 200 ms; Thread 2 starts by sleeping 100 ms.",
    table: [R(1, 0), R(2, 1)], added: 2 },
  { t: 100, msg: "Thread 2 wakes up → fork 1 creates PID 3, then sleeps 300 ms.",
    table: [R(1, 0), R(2, 1), R(3, 1)], added: 3 },
  { t: 200, msg: "Thread 1 wakes up → fork 1 creates PID 4.",
    table: [R(1, 0), R(2, 1), R(3, 1), R(4, 1)], added: 4 },
  { t: 200, msg: "Thread 1 → wait 1 -1 returns -1: PID 1 has no zombie child yet (wait is non-blocking). Then ps prints the table.",
    table: [R(1, 0), R(2, 1), R(3, 1), R(4, 1)] },
  { t: 400, msg: "Thread 2 wakes up → exit 2 10: PID 2 becomes a ZOMBIE with status 10. Nobody reaps it, so it stays a zombie. Both workers finish.",
    table: [R(1, 0), { pid: 2, ppid: 1, state: "ZOMBIE", exit: 10 }, R(3, 1), R(4, 1)], added: 2 }
];

let cur = 0;
let timer = null;
const $ = (id) => document.getElementById(id);
const lines = document.querySelectorAll(".script li");

function render() {
  const s = steps[cur];
  $("clock").textContent = `t = ${s.t} ms`;
  $("event").textContent = s.msg;
  $("ptable-body").innerHTML = s.table.map((p) =>
    `<tr class="${p.pid === s.added ? "new" : ""}">
       <td>${p.pid}</td><td>${p.ppid}</td>
       <td><span class="badge ${p.state}">${p.state}</span></td>
       <td>${p.exit}</td>
     </tr>`).join("");
  lines.forEach((li) => {
    const n = Number(li.dataset.step);
    li.classList.toggle("active", n === cur);
    li.classList.toggle("done", n < cur);
  });
  $("progress-bar").style.width = `${(cur / (steps.length - 1)) * 100}%`;
  $("prev").disabled = cur === 0;
  $("next").disabled = cur === steps.length - 1;
}

function stop() {
  clearInterval(timer);
  timer = null;
  $("play").textContent = "⟲ Auto-play";
}

$("next").onclick = () => { stop(); if (cur < steps.length - 1) { cur++; render(); } };
$("prev").onclick = () => { stop(); if (cur > 0) { cur--; render(); } };
$("play").onclick = () => {
  if (timer) return stop();
  cur = 0; render();
  $("play").textContent = "■ Stop";
  timer = setInterval(() => {
    if (cur >= steps.length - 1) return stop();
    cur++; render();
  }, 2200);
};

render();
