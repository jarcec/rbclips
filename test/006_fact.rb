require 'test/unit'
require 'rbclips'

class Test_Fact < Test::Unit::TestCase
  def test_exists
    assert Clips.constants.member?(:Fact)
  end

  def test_ordered
    assert_nothing_raised   { Clips::Fact.new 'human', [:age, 'name', 30, 1.5] }
    assert_nothing_raised   { Clips::Fact.new 'human', %w(jedna dva tri ctyri) }

    a = Clips::Fact.new 'human', [:jarcec, 22]

    assert a.save
    assert !a.save
    assert a.destroy!
    assert !a.destroy!

    assert a.ordered?
  end

  def test_clone
    a = Clips::Fact.new 'human', %w(x y z)
    b = a.clone
    c = b.dup

    assert a.eql? b
    assert b.equal? c
    assert a == c

    a.save
    assert ! ( a.eql? b )
    assert ! ( a.equal? b )
    assert ! ( a == b )
    assert b == c
  end

  def test_slots
    a = Clips::Fact.new 'human', %w(x y z)
    assert a.slots
    assert_equal a.slots, ['x', 'y', 'z']

    a = Clips::Fact.new 'human', [30, :ahoj, 'zdar', 4.2]
    assert_equal a.slots, [30, :ahoj, 'zdar', 4.2]
  end
end
